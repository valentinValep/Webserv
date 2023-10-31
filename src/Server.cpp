/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:41 by chmadran          #+#    #+#             */
/*   Updated: 2023/10/31 16:52:17 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <stdio.h>


Server::Server(const ConfigSettings& _settings) : settings(_settings) {
}

void Server::start() {
	int server_fd;
	int new_socket;
	long valread;
	struct sockaddr_in address;

	int addrlen = sizeof(address);
	// const char* hello = "Hello from server";

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

	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");

		if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		std::vector<char> buffer(settings.max_client_body_size, '\0');
		valread = read(new_socket, buffer.data(), settings.max_client_body_size);
		printf("%s\n", buffer.data());
		std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "\r\n"
                           "<html><body><h1>Hello World!</h1></body></html>";
		write(new_socket, httpResponse.c_str(), httpResponse.size());


		close(new_socket);
	}
}
