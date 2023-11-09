/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/09 17:18:42 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"

ServerManager::ServerManager(std::string configFilePath) : request(" ") {
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
	clientRequest.print();
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

#define MAX_CONNECTION 10

void ServerManager::start() {
	setupNetwork();

	int ret;
	int nfds = 1;
	int current_size;
	struct pollfd fds[MAX_CONNECTION];
	
	fds[0].fd = this->server_fd;
	fds[0].events = POLLIN;
	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");

		ret = poll(fds, nfds, 0);

		if (ret == -1)
		{
			perror("In poll");
			exit(EXIT_FAILURE);
		}
		else if (ret == 0)
		{
			std::cout << "Still waiting" << std::endl;
		}
		else
		{
			current_size = nfds;
			for (int i = 0; i < current_size; i++)
			{
				if (fds[i].revents & POLLIN && fds[i].fd == server_fd)
				{
					std::cout << "New conn" << std::endl;
					int clientSocket;
					struct sockaddr_in clientAddress;
					socklen_t clientAddressLength = sizeof(clientAddress);

					if ((clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength)) < 0) {
						perror("In accept");
						exit(EXIT_FAILURE);
					}
					std::cout << "Accept return" << clientSocket << std::endl;
					fds[nfds].fd = clientSocket;
					fds[nfds].events = POLLIN;
					nfds++;
				}
				else if (fds[i].revents & POLLIN)
				{
					std::cout << "Handling" << std::endl;
					handleClientRequest(fds[i].fd);
					fds[i].events = POLLOUT;
				}
				else if (fds[i].revents & POLLOUT)
				{
					std::cout << "Responding" << std::endl;
					ServerResponse serverResponse;
					serverResponse.process(request, fds[i].fd);
					fds[i].events = POLLIN;
					// close(fds[i].fd);
					// --nfds;
				}
			}
		}
		sleep(1);
	}
}