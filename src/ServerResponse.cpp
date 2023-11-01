/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 16:02:39 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerResponse.hpp"

ServerResponse::ServerResponse(){};

void ServerResponse::process(const ClientRequest& request, int clientSocket) {
	if (request.method == "GET") {
		std::string content = readFileContent("src/index.html");
		sendHttpResponse(clientSocket, content);
	} else if (request.method == "POST") {
		// Handle POST request
		// Generate appropriate response
	} else if (request.method == "DELETE") {
		// Handle DELETE request
		// Generate appropriate response
	}
	//ADD ERROR HANDLING HERE
}

std::string ServerResponse::readFileContent(const std::string& filePath) {
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		perror("In opening file");
		exit(EXIT_FAILURE);
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ServerResponse::sendHttpResponse(int clientSocket, const std::string& content) {
	std::string httpResponse = "HTTP/1.1 200 OK\r\n"
							   "Content-Type: text/html\r\n"
							   "\r\n" +
							   content;
	write(clientSocket, httpResponse.c_str(), httpResponse.size());
}