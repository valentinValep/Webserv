/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/25 20:39:08 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"
#include "utils.hpp"
#include <iostream>

#define NO_EVENT 0
#define TIMEOUT 120
#define CR std::cout << std::endl;

std::vector<Server>	parseConfigFile(std::string config_file);

/************************************************************
 *					SINGLETON STUFF							*
 ************************************************************/

ServerManager *ServerManager::_instance = NULL;

ServerManager::ServerManager(std::string configFilePath): servers(parseConfigFile(configFilePath)), reactor(this->servers)
{}

ServerManager *ServerManager::getInstance(std::string config_file)
{
	if (!ServerManager::_instance)
		ServerManager::_instance = new ServerManager(config_file);
	return (ServerManager::_instance);
}

ServerManager::~ServerManager()
{
	delete ServerManager::_instance; // @TODO check if it's ok
	ServerManager::_instance = NULL;
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

	// White list
	parser.whitelist("/server/server_name");
	parser.whitelist("/server/client_max_body_size");
	parser.whitelist("/server/autoindex");
	parser.whitelist("/server/error_page");

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

std::vector<Server>	parseConfigFile(std::string config_file)
try {
	fp::FileParser		parser(config_file);
	fp::Module			*mod;
	std::vector<Server>	servers;

	configParser(parser);
	mod = parser.parse();

	for (std::vector<fp::Object *>::const_iterator it = mod->getObjects().begin(); it != mod->getObjects().end(); it++)
	{
		servers.push_back(Server(*dynamic_cast<fp::Module *>(*it)));
	}
	return (servers);
}
catch (const fp::FileParser::FileParserException &e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}
catch(const fp::FileParser::FileParserSyntaxException& e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}

int	ServerManager::addClient(int socket_fd)
{
	return this->reactor.addClient(socket_fd);
}

/************************************************************
 *						RUN									*
 ************************************************************/

void ServerManager::run()
{
	this->reactor.run();
};

//void ServerManager::handleEvent(pollfd &pollfd)
//{
//	if (listeningSockets.find(pollfd.fd) != listeningSockets.end())
//	{
//		acceptNewConnexion(pollfd.fd);
//		return;
//	}
//	if (pollfd.revents & POLLIN)
//	{
//		std::cout << "📩 Handling on [" << pollfd.fd << "]" << std::endl;
//		this->handleClientRequest(this->clientSockets[pollfd.fd].request);
//		// @TODO if request send connection close, close socket after sending response
//		// https://www.rfc-editor.org/rfc/rfc9112.html#name-tear-down
//		if (this->clientSockets[pollfd.fd].request.isClosed())
//		{
//			pollfd.fd = -1;
//			return;
//		}
//		if (this->clientSockets[pollfd.fd].request.isFullyReceived()
//			|| (this->clientSockets[pollfd.fd].request.getErrorCode() == 400))
//			pollfd.events = POLLOUT;
//		this->clientSockets[pollfd.fd].request.short_print();
//	}
//	else if (pollfd.revents & POLLOUT)
//	{
//		std::cout << "📮 Responding on [" << pollfd.fd << "]" << std::endl;
//		ServerResponse serverResponse;
//		serverResponse.prepare(this->clientSockets[pollfd.fd].request);
//		serverResponse.process();
//		if (this->clientSockets[pollfd.fd].request.getErrorCode() == 400)
//		{
//			this->clientSockets[pollfd.fd].request.close();
//			pollfd.fd = -1;
//			return;
//		}
//		else if (this->clientSockets[pollfd.fd].request.isError())
//			this->clientSockets[pollfd.fd].request.hard_reset();
//		else
//			this->clientSockets[pollfd.fd].request.reset();
//		pollfd.events = POLLIN;
//		std::cout << "🟢 Response sent on [" << pollfd.fd << "]" << std::endl;
//	}
//	else
//	{
//		std::cout << "⚠️ Unknown event on [" << pollfd.fd << "] (code: " << pollfd.revents << ")" << std::endl;
//	}
//	this->updateSocketActivity(pollfd.fd);
//}


/************************************************************
 *						NETWORK STUFF						*
 ************************************************************/

//void ServerManager::acceptNewConnexion(int server_fd) {

//	int					clientSocket = 0;
//	struct sockaddr_in	clientAddress;
//	socklen_t			clientAddressLength = sizeof(clientAddress);

//	clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);

//	if (clientSocket < 0)
//	{
//		std::cout << "server_fd: " << server_fd << std::endl;
//		perror(SCSTR(__FILE__ << ":" << __LINE__ << ": In accept"));
//		return ;
//	}

//	if (this->clientSockets.size() >= MAX_CONNECTION) // @TODO move before accept() ?!
//	{
//		std::cout << "\033[93mWarning\033[0m: max number of connections reached" << std::endl;
//		close(clientSocket);
//		return ;
//	}
//	this->clientSockets[clientSocket] = (SocketInfo){ClientRequest(clientSocket, this->listeningSockets[server_fd]), time(NULL)};
//	this->fds.push_back((struct pollfd){clientSocket, POLLIN, NO_EVENT});
//	this->nfds++;

//	std::cout << "✅ New connexion on fd [" << clientSocket << "]" << std::endl;
//	return ;
//};

/************************************************************
 *						REQUEST STUFF						*
 ************************************************************/

//void	ServerManager::handleClientRequest(ClientRequest &request) {
//	ssize_t bytesRead = 0;
//	bytesRead = readClientRequest(request);
//	if (request.isClosed())
//		return;

//	request << std::string(this->buffer, bytesRead);
//	request.parse(this->servers);
//}

//int	ServerManager::readClientRequest(ClientRequest &request) {
//	ssize_t bytesRead = recv(request.getClientSocket(), this->buffer, BUFFER_SIZE, 0);

//	if (bytesRead <= 0) {
//		if (bytesRead < 0)
//			perror(SCSTR(__FILE__ << ":" << __LINE__ << ": In read"));
//		if (bytesRead == 0)
//			std::cout << "❌ Connection closed by client [" <<  request.getClientSocket() << "]" << std::endl;
//		request.close();
//		return (bytesRead);
//	}
//	// std::cout << "\n\n" << "===============   "  << bytesRead << " BYTES  RECEIVED   ===============\n";
//	// for (int i = 0; i < bytesRead; i++)
//	// 	std::cout << this->buffer[i];
//	return (bytesRead);
//}

/************************************************************
 *							DEBUG							*
 ************************************************************/

//void ServerManager::printActiveSockets() {
//	for (std::map<int, SocketInfo>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
//		std::cout << "Socket: " << it->first << " last activity: " << it->second.lastActivity << std::endl;
//	}
//}

//void	ServerManager::updateSocketActivity(int socket) {

//	clientSockets[socket].lastActivity = time(NULL);
//}

//void	ServerManager::detectTimeOut() {
//	time_t currentTime = time(NULL);

//	for (std::map<int, SocketInfo>::iterator it = clientSockets.begin(); it != clientSockets.end(); ++it) {
//		time_t lastActivity = it->second.lastActivity;
//		double secondsSinceLastActivity = difftime(currentTime, lastActivity);

//		if (secondsSinceLastActivity > TIMEOUT) {
//			std::cout << "🕑 Inactive socket [" << it->second.request.getClientSocket() << "] will be removed." << std::endl;
//			 for (std::vector<struct pollfd>::iterator fd_it = fds.begin(); fd_it != fds.end(); ++fd_it) {
//				if (fd_it->fd == it->first) {
//					fd_it->fd = -1;
//					break;
//				}
//			}
//			it->second.request.close();
//		}
//	}
//}

//void ServerManager::cleanFdsAndActiveSockets() {
//	for (std::vector<struct pollfd>::iterator it = this->fds.begin(); it != this->fds.end();)
//	{
//		if (it->fd == -1)
//		{
//			it = this->fds.erase(it);
//			this->nfds--;
//		}
//		else
//			++it;
//	}

//	for (std::map<int, SocketInfo>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end();) {
//		if (it->second.request.getClientSocket() == -1) {
//			std::map<int, SocketInfo>::iterator toErase = it;
//			++it;
//			this->clientSockets.erase(toErase);
//		} else {
//			++it;
//		}
//	}
//}
