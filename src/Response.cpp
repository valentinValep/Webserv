#include "Response.hpp"
#include "ServerManager.hpp"
#include <sstream>

Response::Response(): _code(200)
{}

Response::~Response()
{}

int	Response::getCode() const
{
	return this->_code;
}

void Response::setCode(int code)
{
	this->_code = code;
}

void Response::addHeader(std::string key, std::string value)
{
	this->_headers[key] = value;
}

void Response::setBody(std::string body)
{
	this->_body = body;
}

std::string	Response::build()
{
	std::stringstream	response;

	response << "HTTP/1.1 " << this->_code;
	if (this->_code == 200)
		response << " OK";
	response << "\r\n";

	this->addHeader("Content-Length", SSTR(this->_body.size()));
	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
		response << it->first << ": " << it->second << "\r\n";
	response << "\r\n";

	if (!this->_body.empty())
		response << this->_body;

	return response.str();
}
