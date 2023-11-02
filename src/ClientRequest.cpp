/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/02 14:49:37 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(const std::string& request) {
	// Parse the request
	std::istringstream iss(request);
	iss >> method >> path >> protocol;

	// Read headers
	std::string line;
	while (std::getline(iss, line) && line != "\r") {
		headers.push_back(line);
}

	// Read body
	std::getline(iss, body, '\0');
}

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
