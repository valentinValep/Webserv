/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/10 11:52:30 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"

#define MAX_CONNECTION 3
#define CR std::cout << std::endl;


ServerManager::ServerManager(std::string configFilePath) : request(" "), fds(1) {
	parseConfigFile(configFilePath);
}

void ServerManager::handleClientRequest(int clientSocket) {
	std::vector<char> buffer(10000, '\0');
	ssize_t bytesRead = read(clientSocket, buffer.data(), 10000);

	if (bytesRead < 0) {
		perror("In read");
		close(clientSocket);
		return;
	}

	std::string request(buffer.data(), bytesRead);

	ClientRequest clientRequest(request);
	// clientRequest.print();
	this->request = clientRequest;
}

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
	parser.whitelist("/server/location/extention");
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

void ServerManager::setupNetwork() {
	int opt = 1;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	// @TODO change SOL_SOCKET by TCP protocol (man setsockopt) ? check return < 0 ?
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->servers[0].getPort());

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("In bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 10) < 0) {
		perror("In listen");
		exit(EXIT_FAILURE);
	}
}

int ServerManager::acceptNewConnexion(int server_fd, int &nfds) {

	std::cout << "New connexion" << std::endl;
	int clientSocket = 0;
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (clientSocket < 0)
		return (EXIT_FAILURE);

	if (nfds >= MAX_CONNECTION) {
		//@TODO more sophisticated system to track whos the oldest???
		std::vector<struct pollfd>::iterator oldest_connexion = fds.begin() + 1; // Iterator to the second element
		close(oldest_connexion->fd);
		oldest_connexion->fd = clientSocket;
		oldest_connexion->events = POLLIN;
	} 
	else {
		struct pollfd new_conn;
		new_conn.fd = clientSocket;
		new_conn.events = POLLIN;
		fds.push_back(new_conn);
		nfds++;
	}
	std::cout << "Accept return [" << clientSocket << "]" << std::endl;
	return (EXIT_SUCCESS);
};

void ServerManager::start() {
	setupNetwork();

	int ret;
	int nfds = 1;
	int current_size;

	fds[0].fd = this->server_fd;
	fds[0].events = POLLIN;
	while (1) {

		ret = poll(&fds.front(), nfds, 0);

		if (ret == -1)
		{
			perror("In poll");
			exit(EXIT_FAILURE);
		}
		else if (ret == 0)
		{
			//std::cout << "Still waiting" << std::endl;
		}
		else
		{
			current_size = nfds;
			for (int i = 0; i < current_size; i++)
			{
				if (fds[i].revents & POLLIN && fds[i].fd == server_fd)
				{
					acceptNewConnexion(server_fd, nfds);
				}
				else if (fds[i].revents & POLLIN)
				{
					CR;
					std::cout << "Handling on [" << fds[i].fd << "]" << std::endl;
					close(fds[5].fd);
					handleClientRequest(fds[i].fd);
					fds[i].events = POLLOUT;
				}
				else if (fds[i].revents & POLLOUT)
				{
					CR;
					std::cout << "Responding on [" << fds[i].fd << "]" << std::endl;
					ServerResponse serverResponse;
					serverResponse.process(request, fds[i].fd);
					fds[i].events = POLLIN;
				}
				else
				{
					// std::cout << "fds[i].revents: " << fds[i].revents << std::endl;
				}
			}
		}
		//sleep(1);
	}
}