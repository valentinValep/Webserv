/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/27 21:42:57 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"
#include "ServerManager.hpp"

/************************************************************
 *						CONSTRUCTORS						*
 ************************************************************/

ClientRequest::ClientRequest(): _cgiRequest(false), _port(0), _errorCode(0), _clientSocket(0), _method(0), _state(RECEIVING_METHOD), _server(NULL) {}

ClientRequest::ClientRequest(int fd, in_port_t port): _cgiRequest(false), _port(port), _errorCode(0), _clientSocket(fd), _method(0), _state(RECEIVING_METHOD), _server(NULL) {}


/************************************************************
 *					GETTERS & SETTERS						*
 ************************************************************/

bool ClientRequest::isFullyReceived() const
{
	return (this->_state == REQUEST_FULLY_RECEIVED);
}

bool ClientRequest::isError() const
{
	return (this->_errorCode != 0);
}

in_port_t ClientRequest::getPort() const
{
	return (this->_port);
}

int ClientRequest::getErrorCode() const
{
	return (this->_errorCode);
}

int ClientRequest::getMethod() const
{
	return (this->_method);
}

std::string ClientRequest::getProtocol() const
{
	return (this->_protocol);
}

std::map<std::string,std::string> ClientRequest::getHeaders() const
{
	return (this->_headers);
}

std::string ClientRequest::getPath() const
{
	return (this->_path);
}

Server *ClientRequest::getServer() const
{
	return (this->_server);
}

int ClientRequest::getClientSocket() const
{
	return (this->_clientSocket);
}

std::string ClientRequest::getBodyBody() const
{
	return (this->_body.getBody());
}

bool ClientRequest::isCgiRequest() const
{
	return (this->_cgiRequest);
}

/************************************************************
 *						PARSE								*
 ************************************************************/

void ClientRequest::parseMethodLine(const std::string line)
{
	std::string			method;
	std::istringstream	lines;
	std::string			end;

	//std::cout << "ClientRequest: parseMethodLine: line: '" << line << "'" << std::endl;
	lines.str(line);
	lines >> method >> this->_path >> this->_protocol >> end;
	if (method.empty() || this->_path.empty() || this->_protocol.empty()
		|| !end.empty() || lines.bad() || !lines.eof())
		return this->setError(__FILE__, __LINE__, 400);
	if (method == "GET")
		this->_method = GET;
	else if (method == "POST")
		this->_method = POST;
	else if (method == "DELETE")
		this->_method = DELETE;
	else
		return this->setError(__FILE__, __LINE__, 405);
	if (this->_protocol != HTTP_PROTOCOL && this->_protocol != "undefined")
		return this->setError(__FILE__, __LINE__, 505);
	this->_state = RECEIVING_HEADER;
}

bool	ClientRequest::needBody()
{
	if (this->_method != POST)
		return (false);
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		errno = 0;
		long contentLength = std::strtol(this->_headers["Content-Length"].c_str(), NULL, 10);
		if (contentLength < 0 || errno == ERANGE)
		{
			this->setError(__FILE__, __LINE__, 400);
			return (false);
		}

		this->_body.setContentLength(contentLength);
		return (true);
	}
	this->_body.setChunked();
	return (true);
}

void	ClientRequest::parseHeaderLine(const std::string line)
{
	if (line == "\r")
	{
		if (this->needBody())
			this->_state = RECEIVING_BODY;
		else
			this->_state = REQUEST_FULLY_RECEIVED;
		return;
	}
	if (line.find(":") == std::string::npos)
		return this->setError(__FILE__, __LINE__, 400);
	this->_headers[line.substr(0, line.find(":"))] = line.substr(line.find(":") + 2);
}

void ClientRequest::parseBodyLine(const std::string &line)
{
	if (this->_body.isFinished())
		return;
	try {
		this->_body.parseLine(line + "\n");
	}
	catch (const Body::BodyException &e) {
		return this->setError(__FILE__, __LINE__, 400);
	}
	catch (const Body::BodyTooLargeException &e) {
		return this->setError(__FILE__, __LINE__, 413);
	}
	if (this->_body.isFinished())
	{
		this->_state = REQUEST_FULLY_RECEIVED;
		return;
	}
}

void	ClientRequest::parse()
{
	std::string	line;

	if (!this->_server)
	{
		try
		{
			this->_server = &ServerManager::getInstance()->getServer(this->_port);
		}
		catch(const ServerManager::ServerNotFoundException& e)
		{
			return this->setError(__FILE__, __LINE__, 500);
		}
	}
	while (std::getline(this->_raw_data, line))
	{
		if (this->getErrorCode() == 400)
			return;

		//// #### DEBUG
		//for (std::size_t i = 0; i < line.size(); ++i) {
		//	std::cout << (int)line[i] << " ";
		//}
		//std::cout << std::endl;
		//// #### END DEBUG
		if (this->_raw_data.eof())
		{
			//std::cout << "eof" << std::endl;
			this->_raw_data.clear();
			this->_raw_data.str("");
			this->_raw_data << line;
			return;
		}

		if ((line.empty() || line.size() == 0) && this->_state != RECEIVING_BODY)
			return this->setError(__FILE__, __LINE__, 400);
		//std::cout << "state: " << this->_state << " line: '" << line << "'" << std::endl;
		if (this->_state != RECEIVING_BODY && line[line.size() - 1] != '\r')
			return this->setError(__FILE__, __LINE__, 400);
		if (this->_state == RECEIVING_METHOD)
		{
			this->parseMethodLine(line);
			if (this->getErrorCode() == 400)
				return;
		}
		else if (this->_state == RECEIVING_HEADER)
		{
			this->parseHeaderLine(line);
			if (this->_state != RECEIVING_HEADER)
			{
				this->findFinalServer();
				try
				{
					this->_body.setMaxBodySize(this->_server->getMaxBodySize());
				}
				catch(const Body::BodyTooLargeException& e)
				{
					this->setError(__FILE__, __LINE__, 413);
				}
			}
		}
		else if (this->_state == RECEIVING_BODY)
			this->parseBodyLine(line);
		else
			return;
	}
	this->_raw_data.clear();
	this->_raw_data.str("");
}

void ClientRequest::detectCgi()
{
	std::string pathWithoutQuery = this->_path;
	size_t queryPos = this->_path.find("?");
	if (queryPos != std::string::npos) {
		pathWithoutQuery = this->_path.substr(0, queryPos);
	}

	//	@TODO: Send 404 if dont have script + Searching for extension is not enough. Accessing localhost:8080/hdxfcghbj.py returns respons 200. (Same problem with ,py)
	// @TODO: remove all extensions but .py
	const char* cgiExtensions[] = {".cgi", ".pl", ".py"};	//	Use a Vector<std::string>	//	char * transformed to std::String below
	const size_t numExtensions = sizeof(cgiExtensions) / sizeof(cgiExtensions[0]);	//	useless

	for (size_t i = 0; i < numExtensions; ++i) {
		std::string ext = cgiExtensions[i];
		if (pathWithoutQuery.size() >= ext.size() && pathWithoutQuery.compare(pathWithoutQuery.size() - ext.size(), ext.size(), ext) == 0) {
			this->_cgiRequest = true;
			return;
		}
	}
	this->_cgiRequest = false;
}

void ClientRequest::findFinalServer()
{
	std::string			port_str;
	std::string			host_name;
	int					port;
	size_t				pos;

	if (this->_headers.find("Host") == this->_headers.end())
		return this->setError(__FILE__, __LINE__, 400);
	pos = this->_headers["Host"].find(":");
	if (pos == std::string::npos)
	{
		host_name = this->_headers["Host"];
		port = 80;
	}
	else
	{
		host_name = this->_headers["Host"].substr(0, pos);
		port_str = this->_headers["Host"].substr(pos + 1); // 0 < x < 65534 (short max value)
		std::istringstream iss(port_str);
		iss >> port_str;
		if (port_str.empty() || port_str.length() > 5)
			return this->setError(__FILE__, __LINE__, 400);
		port = atoi(port_str.c_str());
	}
	if (port != this->_port)
		return this->setError(__FILE__, __LINE__, 400);
	try
	{
		this->_server = &ServerManager::getInstance()->getServer(this->_port, host_name);
	}
	catch(const ServerManager::ServerNotFoundException& e)
	{}
}

std::string ClientRequest::getHeader(const std::string & key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
	if (it == this->_headers.end())
		return ("");
	return (it->second);
}

void ClientRequest::reset()
{
	this->_cgiRequest = false;
	this->_errorCode = 0;
	this->_method = 0;
	this->_state = RECEIVING_METHOD;
	this->_path.clear();
	this->_protocol.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_raw_data.clear();
	this->_raw_data.str("");
}

void ClientRequest::hard_reset()
{
	this->reset();
	this->_server = NULL;
}

void ClientRequest::operator<<(const std::string &data)
{
	//std::cout << "ClientRequest: operator<<: _raw_data: '" << this->_raw_data.str() << "'" << std::endl;
	this->_raw_data << data;
}

/************************************************************
 *						PRINT								*
 ************************************************************/

void ClientRequest::setError(std::string file, int line, int errorCode)
{
	(void)file;
	(void)line;
	//std::cout << "Debug: " << file << ":" << line << ": ClientRequest: setError: errorCode: " << errorCode << std::endl;
	this->_errorCode = errorCode;
}

void ClientRequest::print() const {
	if (this->isError()) {
		std::cout << "ClientRequest: Error: " << this->_errorCode << std::endl;
		return;
	}
	if (this->_state != REQUEST_FULLY_RECEIVED) {
		std::cout << "ClientRequest: not fully received" << std::endl;
		return;
	}
	if (this->_state == RECEIVING_BODY) {
		std::cout << "ClientRequest: receiving body" << std::endl;
		return;
	}
	std::cout << "ClientRequest: " << std::endl;
	std::cout << "Method: " << (this->_method == GET ? "GET" : this->_method == POST ? "POST"
																					 : "DELETE")
			  << std::endl;
	std::cout << "Path: " << this->_path << std::endl;
	std::cout << "Protocol: " << this->_protocol << std::endl;

	std::cout << "Headers:" << std::endl;
	for(std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	}

	//std::cout << "Body:" << std::endl;
	//std::cout << this->_body << std::endl;
}

void ClientRequest::short_print() const {
	if (this->isError()) {
		std::cout << "ClientRequest: Error: " << this->_errorCode << std::endl;
		return;
	}
	if (this->_state != REQUEST_FULLY_RECEIVED) {
		std::cout << "ClientRequest: request parse is not finished" << std::endl;
		return;
	}
	std::cout << "ClientRequest: " << (this->_method == GET ? "GET " : this->_method == POST ? "POST "
																					 : "DELETE ");
	std::cout << this->_path << std::endl;
}
