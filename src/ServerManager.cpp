/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/13 17:35:22 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"

#define MAX_CONNECTION 10
#define NO_EVENT 0
#define CR std::cout << std::endl;


/************************************************************
 *					CONSTRUCTOR								*
 ************************************************************/

ServerManager::ServerManager(std::string configFilePath) : nfds(0), request(" "), fds(1) {
	parseConfigFile(configFilePath);
	if (setupNetwork() == EXIT_SUCCESS)
	{
		std::cout << "Set up complete of " << servers.size() << " servers." << std::endl;
		printServersSockets();
		start();
	}
}


/************************************************************
 *						PARSER								*
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

/************************************************************
 *						LAUNCHER							*
 ************************************************************/

void ServerManager::start() {
	
	int ret;

	while (1) {
		ret = poll(&fds.front(), nfds, 0);
		if (ret == -1)
		{
			perror("In poll");
			exit(EXIT_FAILURE);
		}
		for (int index = 0; index < nfds; index++)
		{
			if (fds[index].revents == NO_EVENT)
				continue;
			handleEvent(index);
		}
	}
};

void ServerManager::updateFds(size_t len, std::vector<SocketInfo>& clientSockets){
	for (size_t i = len; i < clientSockets.size(); i++)
	{
		fds[nfds].fd =clientSockets[i].socket;
		fds[nfds].events = POLLIN;
		fds[nfds].revents = NO_EVENT;
		nfds++;
	}
};

void ServerManager::handleEvent(size_t i)
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		Server &currentServer = *it;
		std::vector<SocketInfo>& clientSockets = currentServer.getClientSockets();
		// size_t len = clientSockets.size();
		
		if (fds[i].fd == currentServer.server_fd)
		{
			// currentServer.acceptNewConnections();
			// updateFds(len, clientSockets);
			acceptNewConnexion(fds[i].fd);
		}
		
		bool foundSocket = false;
		for (size_t k = 0; k < clientSockets.size(); k++) {
			if (clientSockets[k].socket == fds[i].fd) {
				foundSocket = true;
				break;
			}
		}
		if (foundSocket && (fds[i].revents & POLLIN)) {
			std::cout << "Handling on [" << fds[i].fd << "]" << std::endl;
			handleClientRequest(fds[i].fd);
			fds[i].events = POLLOUT;
		}
		if (foundSocket && (fds[i].revents & POLLOUT)) {
			std::cout << "Responding on [" << fds[i].fd << "]" << std::endl;
			ServerResponse serverResponse;
			serverResponse.process(request, fds[i].fd);
			fds[i].events = POLLIN;
		}
		updateSocketActivity(fds[i].fd);
		// detectInactiveSockets();
		// cleanFdsAndActiveSockets(nfds);
	}
}


/************************************************************
 *						METHODS								*
 ************************************************************/

int ServerManager::setupNetwork() {
	for (size_t i = 0; i < servers.size(); i++)
	{
		struct sockaddr_in address;
		memset(address.sin_zero, '\0', sizeof address.sin_zero);
		address = this->servers[i].address;
		
		int opt = 1;
		int serverSocket = 0;
		if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
			perror("In socket");
			exit(EXIT_FAILURE);
		}

		// @TODO change SOL_SOCKET by TCP protocol (man setsockopt) ? check return < 0 ?
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
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
		fds.push_back(new_server_fd);
		servers[i].server_fd = serverSocket;
		nfds++;
	}
	return (EXIT_SUCCESS);
}

int ServerManager::acceptNewConnexion(int server_fd) {

	int clientSocket = 0;
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	
	if (clientSocket < 0)
		return (EXIT_FAILURE);

	for (size_t i = 0; i < servers.size(); ++i) {
		if (servers[i].server_fd == server_fd) {
			if (servers[i].getClientSockets().size() >= MAX_CONNECTION) {
				std::cout << "warning: max number of connections reached for server on fd [" << server_fd << "]" << std::endl;
				close(clientSocket);
				return (EXIT_FAILURE);
			} else {
				servers[i].addClientSocket(clientSocket);

				struct pollfd new_connexion;
				new_connexion.fd = clientSocket;
				new_connexion.events = POLLIN;
				fds.push_back(new_connexion);

				nfds++;
			}

			break;
		std::cout << "New connexion on fd [" << clientSocket << "]" << std::endl;
		return (EXIT_SUCCESS);
		}
	}
	return (EXIT_FAILURE);
};

/************************************************************
 *						READ / WRITE						*
 ************************************************************/

void ServerManager::handleClientRequest(int clientSocket) {
	std::vector<char> buffer(10000, '\0');
	ssize_t bytesRead = read(clientSocket, buffer.data(), 10000);

	if (bytesRead < 0) {
		perror("In read");
		close(clientSocket);
		return;
	}

	if (bytesRead == 0) {
		std::cout << "connection closed by client [" <<  clientSocket << "]" << std::endl;
		close(clientSocket);
		clientSocket = -1;
		return;
	}

	std::cout << "\n\n" << "===============   "  << bytesRead << " BYTES  RECEIVED   ===============\n";
	for (int i = 0; i < bytesRead; i++)
	{
		if (buffer[i] == '\r')
			std::cout << "\\r";
		std::cout << buffer[i];
	}

	std::string request(buffer.data(), bytesRead);
	ClientRequest clientRequest(request);
	this->request = clientRequest;
}

void	ServerManager::updateSocketActivity(int socket) {
	for (size_t i = 0; i < servers.size(); i++) {
		std::vector<SocketInfo>& clientSockets = servers[i].getClientSockets();
		for (std::vector<SocketInfo>::iterator it = clientSockets.begin(); it != clientSockets.end(); ++it) {
			if (it->socket == socket) {
				servers[i].updateClientSocketActivity(socket);
				return;
			}
		}
	}
}

/************************************************************
 *						PRINT DEBUG							*
 ************************************************************/

void ServerManager::printAllServersActiveSockets() {
	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << "================ SERVER " << i + 1 << " =================\n";
		servers[i].printActiveSockets();
		std::cout << std::endl;
	}
}

void ServerManager::printServersSockets() {
	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << "================ SERVER " << i + 1 << " =================\n";
		servers[i].printListeningSocket();
		std::cout << std::endl;
	}
}