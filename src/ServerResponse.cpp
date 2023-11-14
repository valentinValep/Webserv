/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 19:36:39 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include <sstream>

ServerResponse::ServerResponse(){};

void ServerResponse::process(const ClientRequest& request, int clientSocket) {
	if (request.errorCode)
	{
		;//	respond error
		return;
	}
	if (request.method == GET) {
		std::string content;
		if (request.path == "/")
		{
			content = readFileContent("src/" + request.server->getIndex());
			sendHttpResponse(clientSocket, content, "text/html");
		}
		else
		{
			content = readFileContent("src/" + request.path);
			sendHttpResponse(clientSocket, content, "text/" + request.path.substr(request.path.find_last_of(".") + 1));
		}
//		if (request.path == "/style.css")
//		{
//			content = readFileContent("src/style.css");
//			sendHttpResponseCSS(clientSocket, content);
//		}
//		else
//		{
//			content = readFileContent("src/index.html");
//			sendHttpResponse(clientSocket, content);
//		}
	} else if (request.method == POST) {
		// Handle POST request
		// Generate appropriate response
	} else if (request.method == DELETE) {
		// Handle DELETE request
		// Generate appropriate response
	}
	//ADD ERROR HANDLING HERE
}

std::string ServerResponse::readFileContent(const std::string& filePath) {
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		perror("In opening file"); // @TODO return 404
		//exit(EXIT_FAILURE); // @TODO return 404
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ServerResponse::sendHttpResponse(int clientSocket, const std::string& content, const std::string& contentType) {
	std::stringstream httpHeaders;
	httpHeaders << "HTTP/1.1 200 OK\r\n" <<
				"Content-Length: " << content.size() << "\r\n" <<
				"Content-Type: " << contentType << "\r\n" <<
				"\r\n";
	std::string httpResponse = httpHeaders.str() + content;
	write(clientSocket, httpResponse.c_str(), httpResponse.size());
}
