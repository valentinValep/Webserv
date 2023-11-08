/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/08 19:00:26 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"

ServerManager::ServerManager(std::string configFilePath) {
	parseConfigFile(configFilePath);
}

void ServerManager::setupNetwork() {
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

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

	ServerResponse serverResponse;
	serverResponse.process(request, clientSocket);

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


void ServerManager::start() {
	setupNetwork();

	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");

		int clientSocket;
		struct sockaddr_in clientAddress;
		socklen_t clientAddressLength = sizeof(clientAddress);

		if ((clientSocket = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		handleClientRequest(clientSocket);

		close(clientSocket);
	}
}