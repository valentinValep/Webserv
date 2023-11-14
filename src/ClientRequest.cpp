/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 14:22:42 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

/************************************************************
 *						CONSTRUCTORS						*
 ************************************************************/

ClientRequest::ClientRequest(): _clientSocket(0), server(NULL) {}

ClientRequest::ClientRequest(int fd) : _clientSocket(fd), server(NULL) {}


/************************************************************
 *						SETTERS								*
 ************************************************************/

void ClientRequest::setState(State newState)
{
	state = newState;
}

void	ClientRequest::setHeaderInfos(std::string _header, int _headerLen) {
	header = _header;
	headerLen = _headerLen;
};

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
