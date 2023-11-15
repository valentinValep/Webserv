/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/15 11:56:34 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"
#include "utils.hpp"

#define MAX_CONNECTION 10
#define NO_EVENT 0
#define CR std::cout << std::endl;


/************************************************************
 *					CONSTRUCTOR								*
 ************************************************************/

ServerManager::ServerManager(std::string configFilePath): nfds(0), fds()
{
	parseConfigFile(configFilePath);
	if (setupNetwork() == EXIT_SUCCESS)
	{
		std::cout << "Set up complete of " << this->servers.size() << " servers." << std::endl;
		std::cout << "Listening at: " << std::endl;
		for (std::set<int>::iterator it = this->listeningSockets.begin(); it != this->listeningSockets.end(); it++)
			std::cout << "Fd: " << *it << std::endl;
		this->start();
	}
}


/************************************************************
 *						INIT								*
 ************************************************************/

void	configParser(fp::FileParser &parser)
{
	// Requirement
	parser.require("/server");
	parser.require("/server/listen");
	parser.require("/server/root");
	parser.require("/server/index");
	parser.require("/server/allow_methods");
	parser.require("/server/error_page"); // @TODO require error_page code (404, 500, etc)

	// White list
	parser.whitelist("/server/server_name");
	parser.whitelist("/server/client_max_body_size");
	parser.whitelist("/server/autoindex");

	parser.whitelist("/server/location/allow_methods");
	parser.whitelist("/server/location/root");
	parser.whitelist("/server/location/index");
	parser.whitelist("/server/location/autoindex");
	parser.whitelist("/server/location/extension");
	parser.whitelist("/server/location/cgi_path");
	parser.whitelist("/server/location/upload_path");
	parser.whitelist("/server/location/return");

	parser.banVariableValue();
	parser.forceModuleName();
}

void	ServerManager::parseConfigFile(std::string config_file)
try {
	fp::FileParser parser(config_file);
	fp::Module *mod;

	configParser(parser);
	mod = parser.parse();

	for (std::vector<fp::Object *>::const_iterator it = mod->getObjects().begin(); it != mod->getObjects().end(); it++)
	{
		this->servers.push_back(Server(*dynamic_cast<fp::Module *>(*it)));
	}
}
catch(const fp::FileParser::FileParserException& e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}
catch(const fp::FileParser::FileParserSyntaxException& e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}

int ServerManager::setupNetwork() {
	std::set<int>		listeningPorts;
	struct sockaddr_in	address;
	int					serverSocket = 0;

	for (std::vector<Server>::iterator it = this->servers.begin(); it != this->servers.end(); it++)
	{
		if (listeningPorts.find(it->getPort()) != listeningPorts.end())
			continue;
		memset(address.sin_zero, '\0', sizeof address.sin_zero);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(it->getPort());

		if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("In socket");
			exit(EXIT_FAILURE);
		}

		int opt = 1;
		// @TODO change SOL_SOCKET by TCP protocol (man setsockopt) ? check return < 0 ?
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int))) {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
			perror("In bind");
			exit(EXIT_FAILURE);
		}

		if (listen(serverSocket, 10) < 0) {
			perror("In listen");
			exit(EXIT_FAILURE);
		}

		struct pollfd new_server_fd;
		new_server_fd.fd = serverSocket;
		new_server_fd.events = POLLIN;
		this->fds.push_back(new_server_fd); // @TODO compound literal
		this->listeningSockets.insert(serverSocket);
		listeningPorts.insert(it->getPort());
		this->nfds++;
	}
	return (EXIT_SUCCESS);
}

/************************************************************
 *						LAUNCHER							*
 ************************************************************/

void ServerManager::start()
{
	int ret;

	while (1) {
		ret = poll(&this->fds.front(), this->nfds, 5000);
		if (ret == -1)
		{
			perror("In poll");
			exit(EXIT_FAILURE);
		}
		for (int index = 0; index < this->nfds; index++)
		{
			//std::cout << "Checking [" << this->fds[index].fd << "] == " << this->fds[index].revents << std::endl;
			if (this->fds[index].revents == NO_EVENT)
				continue;
			//std::cout << "Event on [" << this->fds[index].fd << "]" << std::endl;
			handleEvent(this->fds[index]);
		}
	}
};

void ServerManager::handleEvent(pollfd &pollfd)
{
	if (listeningSockets.find(pollfd.fd) != listeningSockets.end())
	{
		int ret = acceptNewConnexion(pollfd.fd);
		(void) ret;
		// @TODO check return value
		return;
	}
	if (pollfd.revents & POLLIN)
	{
		int ret = 0;
		std::cout << "Handling on [" << pollfd.fd << "]" << std::endl;
		ret = handleClientRequest(this->clientSockets[pollfd.fd].request);
		if (!ret && this->clientSockets[pollfd.fd].request.state == ClientRequest::REQUEST_FULLY_RECEIVED)
		{
			pollfd.events = POLLOUT;
			this->clientSockets[pollfd.fd].request.state = ClientRequest::HEADER_NOT_FULLY_RECEIVED;
		}
	}
	if (pollfd.revents & POLLOUT)
	{
		std::cout << "Responding on [" << pollfd.fd << "]" << std::endl;
		ServerResponse serverResponse;
		serverResponse.prepare(this->clientSockets[pollfd.fd].request);
		serverResponse.process();
		pollfd.events = POLLIN;
	}
	updateSocketActivity(pollfd.fd);
	// detectInactiveSockets();
	cleanFdsAndActiveSockets();
}


/************************************************************
 *						NETWORK STUFF						*
 ************************************************************/

void ServerManager::cleanFdsAndActiveSockets() {
	for (std::vector<struct pollfd>::iterator it = this->fds.begin(); it != this->fds.end();)
	{
		if (it->fd == -1)
		{
			it = this->fds.erase(it);
			this->nfds--;
			// @TODO remove from clientSockets && detect inactive sockets otherwise no fd ever gets cleaned except one that closed
		}
		else
			++it;
	}

	for (std::map<int, SocketInfo>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end();) {
		if (it->second.request._clientSocket == -1) {
			std::map<int, SocketInfo>::iterator toErase = it;
			++it;
			this->clientSockets.erase(toErase);
		} else {
			++it;
		}
	}
}

int ServerManager::acceptNewConnexion(int server_fd) {

	int					clientSocket = 0;
	struct sockaddr_in	clientAddress;
	socklen_t			clientAddressLength = sizeof(clientAddress);

	// @TODO loop over accept() until no more pending connections (Check non blocking accept) (limit to N simultaneous connections)
	clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);

	// @TODO check Linux specific errno
	if (clientSocket < 0)
	{
		perror("In accept");
		return (EXIT_FAILURE);
	}

	if (this->clientSockets.size() >= MAX_CONNECTION)
	{
		std::cout << "warning: max number of connections reached" << std::endl;
		close(clientSocket);
		return (EXIT_FAILURE);
	}
	this->clientSockets[clientSocket] = (SocketInfo){ClientRequest(clientSocket), time(NULL)};
	this->fds.push_back((struct pollfd){clientSocket, POLLIN, NO_EVENT});
	this->nfds++;

	std::cout << "New connexion on fd [" << clientSocket << "]" << std::endl;
	return (EXIT_SUCCESS);
};

/************************************************************
 *						REQUEST STUFF						*
 ************************************************************/

int ServerManager::handleClientRequest(ClientRequest &request) {

	ssize_t bytesRead = 0;
	bytesRead = readClientRequest(request);

	if (bytesRead > 0 && request.state == ClientRequest::HEADER_NOT_FULLY_RECEIVED)
	{
		storeHeaderClientRequest(buffer, bytesRead, request);
		return (EXIT_SUCCESS);
	}
	if (bytesRead > 0 && request.state == ClientRequest::BODY_NOT_FULLY_RECEIVED)
	{
		storeBodyClientRequest(buffer, bytesRead, request);
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

int ServerManager::readClientRequest(ClientRequest &request) {
	ssize_t bytesRead = recv(request._clientSocket, buffer, BUFFER_SIZE, 0);

	if (bytesRead <= 0) {
		// @TODO better handling of error here
		if (bytesRead < 0)
			perror("In read");
		if (bytesRead == 0)
			std::cout << "connection closed by client [" <<  request._clientSocket << "]" << std::endl;
		request._clientSocket = -1;
		close(request._clientSocket);
		return (bytesRead);
	}
	std::cout << "\n\n" << "===============   "  << bytesRead << " BYTES  RECEIVED   ===============\n";
	for (int i = 0; i < bytesRead; i++)
		std::cout << buffer[i];
	return (bytesRead);
}

void ServerManager::storeHeaderClientRequest(char *buffer, int bytesRead, ClientRequest &request) {
	request.raw_data += std::string(buffer, bytesRead);
	if (containsEmptyLine(request.raw_data))
	{
		std::cout << "end of header found.\n";
		request.setState(ClientRequest::HEADER_FULLY_RECEIVED);

		//std::cout << "HEADER : " << request.raw_data << std::endl;
		//std::cout << "HEADER LENGTH : " << request.raw_data.length() << std::endl;

		request.parseHeader(this->servers);
		request.setBodyState();
		request.raw_data = "";
	}
	else
		request.setState(ClientRequest::HEADER_NOT_FULLY_RECEIVED);
}

void ServerManager::storeBodyClientRequest(char *buffer, int bytesRead, ClientRequest &request) {
	request.raw_data += std::string(buffer, bytesRead);
	request.setBodyState();
	if (request.state == ClientRequest::REQUEST_FULLY_RECEIVED) {
		request.parseBody();
		request.raw_data = "";
	}
	else
		request.setState(ClientRequest::BODY_NOT_FULLY_RECEIVED);
}

/************************************************************
 *							DEBUG							*
 ************************************************************/

void ServerManager::printActiveSockets() {
	for (std::map<int, SocketInfo>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
		std::cout << "Socket: " << it->first << " last activity: " << it->second.lastActivity << std::endl;
	}
}

void	ServerManager::updateSocketActivity(int socket) {

	clientSockets[socket].lastActivity = time(NULL);
}