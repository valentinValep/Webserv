/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/16 16:15:52 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include "Route.hpp"
#include <sstream>

ServerResponse::ServerResponse() : _autoindex(false), _error_code(0), _method(0), _redirect_type(0)
{}

void ServerResponse::setError(int errorCode)
{
	this->_error_code = errorCode;
}

void ServerResponse::prepare(const ClientRequest &request)
{
	if (!request.getServer())
		return this->setError(500);

	const Route	*route = request.getServer()->getRoute(request.getPath());

	// ## Request ##
	// Client socket
	this->_client_socket = request.getClientSocket();
	// Error pages
	this->_error_pages = request.getServer()->getErrorPages();
	if (request.getErrorCode())
		return this->setError(request.getErrorCode());
	// Method
	if (route && route->hasMethods())
		this->_method = route->getMethods() & request.getMethod();
	else
		this->_method = request.getServer()->getMethods() & request.getMethod();
	if (!this->_method)
		return this->setError(405);
	if (request.getProtocol() != HTTP_PROTOCOL)
		return this->setError(505);
	this->_path = request.getPath();
	this->_headers = request.getHeaders();

	// ## Server ##
	// Autoindex
	if (route && route->hasAutoindex())
		this->_autoindex = route->getAutoindex();
	else
		this->_autoindex = request.getServer()->getAutoindex();
	// Root
	if (route && route->hasRoot())
		this->_root = route->getRoot();
	else
		this->_root = request.getServer()->getRoot();
	// Index
	if (route && route->hasIndex())
		this->_index = route->getIndex();
	else
		this->_index = request.getServer()->getIndex();
	if (route)
	{
		// Redirect
		if (route->hasRedirect())
		{
			this->_redirect_type = route->getRedirectType();
			this->_redirect = route->getRedirect();
		}
		// CGI
		if (route->hasCgi())
		{
			this->_cgi_extension = route->getCgiExtension();
			this->_cgi_path = route->getCgiPath();
		}
		// Upload
		if (route->hasUpload())
			this->_upload_path = route->getUploadPath();
	}
}

void ServerResponse::process()
{
	if (this->_error_code)
	{
		;//	respond error
		return;
	}
	if (this->_method == GET) {
		std::string content;
		if (this->_path == "/")
		{
			content = readFileContent("src/" + this->_index);
			sendHttpResponse(this->_client_socket, content, "text/html");
		}
		else
		{
			content = readFileContent("src/" + this->_path);
			sendHttpResponse(this->_client_socket, content, "text/" + this->_path.substr(this->_path.find_last_of(".") + 1));
		}
	} else if (this->_method == POST) {
		// Handle POST request
		// Generate appropriate response
	} else if (this->_method == DELETE) {
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
