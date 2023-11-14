/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 19:28:59 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"
#include "ServerManager.hpp"

/************************************************************
 *						CONSTRUCTORS						*
 ************************************************************/

ClientRequest::ClientRequest(): errorCode(0), _clientSocket(0), server(NULL), method(0) {}

ClientRequest::ClientRequest(int fd) : errorCode(0), _clientSocket(fd), server(NULL), method(0) {}


/************************************************************
 *						SETTERS								*
 ************************************************************/

void ClientRequest::setState(State newState)
{
	state = newState;
}

/************************************************************
 *						PARSE								*
 ************************************************************/

// @TODO rename
void ClientRequest::parse(std::vector<Server> &servers)
{
	std::string			method;
	std::string			line;
	std::istringstream	lines;
	std::istringstream	iss(this->raw_data);

	std::getline(iss, line);
	lines.str(line);
	lines >> method >> this->path >> this->protocol;
	if (method.empty() || this->path.empty() || this->protocol.empty())
	{
		this->errorCode = 400;
		return;
	}
	if (method == "GET")
		this->method = GET;
	else if (method == "POST")
		this->method = POST;
	else if (method == "DELETE")
		this->method = DELETE;
	else
	{
		this->errorCode = 405;
		return;
	}

	// Read headers
	while (std::getline(iss, line) && line != "\r")
	{
		if (line.find(":") == std::string::npos)
		{
			this->errorCode = 400;
			return;
		}
		this->headers[line.substr(0, line.find(":"))] = line.substr(line.find(":") + 2);
	}

	this->findServer(servers);
	if (this->errorCode != 0)
		return;

	// Read body
	this->body = iss.rdbuf()->str();
}

void ClientRequest::findServer(std::vector<Server> &servers)
{
	std::string			port_str;
	std::string			host_name;
	int					port;
	size_t				pos;

	if (this->headers.find("Host") == this->headers.end())
	{
		this->errorCode = 400;
		return;
	}
	pos = this->headers["Host"].find(":");
	if (pos == std::string::npos)
	{
		host_name = this->headers["Host"];
		port = 80;
	}
	else
	{
		host_name = this->headers["Host"].substr(0, pos);
		port_str = this->headers["Host"].substr(pos + 1); // 0 < x < 65534
		std::istringstream iss(port_str);
		iss >> port_str;
		if (port_str.empty() || port_str.length() > 5)
		{
			this->errorCode = 400;
			return;
		}
		port = atoi(port_str.c_str());
	}
	std::cout << "host_name: '" << host_name << "'" << std::endl;
	std::cout << "port: '" << port << "'" << std::endl;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->hasServerName(host_name) && it->getPort() == port)
		{
			this->server = &(*it);
			return;
		}
	}
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->getPort() == port)
		{
			this->server = &(*it);
			return;
		}
	}
	this->errorCode = 404;
	return;
}


/************************************************************
 *						PRINT								*
 ************************************************************/

void ClientRequest::print() const {
	std::cout << "Method: " << (method == GET ? "GET" : method == POST ? "POST" : "DELETE") << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Protocol: " << protocol << std::endl;

	std::cout << "Headers:" << std::endl;
	for(std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}

	std::cout << "Body:" << std::endl;
	std::cout << body << std::endl;
}
