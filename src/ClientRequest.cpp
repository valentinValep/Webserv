/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 16:08:58 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

/************************************************************
 *						CONSTRUCTORS						*
 ************************************************************/

ClientRequest::ClientRequest(): _clientSocket(0), server(NULL), state(HEADER_NOT_FULLY_RECEIVED) {}

ClientRequest::ClientRequest(int fd) : _clientSocket(fd), server(NULL), state(HEADER_NOT_FULLY_RECEIVED) {}


/************************************************************
 *						SETTERS								*
 ************************************************************/

void ClientRequest::setState(State newState)
{
	state = newState;
}

void	ClientRequest::setHeaderInfos() {
	header = raw_data;
	headerLen = raw_data.length();
};

void	ClientRequest::setBodyInfos() {
	_body = raw_data;
	_bodyLen = raw_data.length();
};

void ClientRequest::setBodyState() {
	if (method == "GET" || method == "HEAD" || method == "DELETE") {
		state = REQUEST_FULLY_RECEIVED;
		return;
	}

	bool hasBody = false;
	for (std::vector<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		if (it->find("Content-Length:") != std::string::npos || 
			it->find("Transfer-Encoding:") != std::string::npos) {
			std::cout << "Body delimiter found" << std::endl;
			hasBody = true;
			break;
		}
	}

	if (hasBody) {
		state = BODY_NOT_FULLY_RECEIVED;
	} else {
		state = REQUEST_FULLY_RECEIVED;
	}
}


/************************************************************
 *						PARSE								*
 ************************************************************/

void ClientRequest::parse()
{
	std::istringstream iss(this->raw_data);
	iss >> method >> path >> protocol;

	// Read headers
	std::string line;
	while (std::getline(iss, line) && line != "\r") {
		headers.push_back(line);
	}

	// Read body
	std::getline(iss, body, '\0');
}


/************************************************************
 *						PRINT								*
 ************************************************************/

void ClientRequest::print() const {
	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Protocol: " << protocol << std::endl;

	std::cout << "Headers:" << std::endl;
	for(std::vector<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		std::cout << *it << std::endl;
	}

	std::cout << "Body:" << std::endl;
	std::cout << body << std::endl;
}
