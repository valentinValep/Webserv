/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/02 14:48:05 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

ServerManager::ServerManager() {
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