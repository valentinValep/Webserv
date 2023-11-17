/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/17 21:14:08 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include "Route.hpp"
#include <sstream>

ServerResponse::ServerResponse(): _autoindex(false), _error_code(0), _method(0), _redirect_type(0)
{}

void ServerResponse::prepare(const ClientRequest &request)
{
	if (!request.server)
	{
		this->_error_code = 500;
		return ;
	}

	const Route	*route = request.server->getRoute(request.getPath());

	// ## Request ##
	// Client socket
	this->_client_socket = request.getClientSocket();
	// Error pages
	this->_error_pages = request.server->getErrorPages();
	if (request.getErrorCode())
	{
		this->_error_code = request.getErrorCode();
		return ;
	}
	// Method
	if (route && route->hasMethods())
		this->_method = route->getMethods() & request.getMethod();
	else
		this->_method = request.server->getMethods() & request.getMethod();
	if (!this->_method)
	{
		this->_error_code = 405;
		return ;
	}
	if (request.protocol != HTTP_PROTOCOL)
	{
		this->_error_code = 505;
		return ;
	}
	this->_path = request.getPath();
	this->_headers = request.getHeaders();

	// ## Server ##
	// Autoindex
	if (route && route->hasAutoindex())
		this->_autoindex = route->getAutoindex();
	else
		this->_autoindex = request.server->getAutoindex();
	// Root
	if (route && route->hasRoot())
		this->_root = route->getRoot();
	else
		this->_root = request.server->getRoot();
	// Index
	if (route && route->hasIndex())
		this->_index = route->getIndex();
	else
		this->_index = request.server->getIndex();
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

std::string		ServerResponse::_getGenericErrorPage(void) const
{
	static std::map<int, std::string> 	error_codes;
	static std::stringstream			generic_page;

	error_codes[400] = "Bad Request";
	error_codes[401] = "Unauthorized";
	error_codes[402] = "Payment Required";
	error_codes[403] = "Forbidden";
	error_codes[404] = "Not Found";
	error_codes[405] = "Method Not Allowed";
	error_codes[406] = "Not Acceptable";
	error_codes[407] = "Proxy Authentication Required";
	error_codes[408] = "Request Time-out";
	error_codes[409] = "Conflict";
	error_codes[410] = "Gone";
	error_codes[411] = "Length Required";
	error_codes[412] = "Precondition Failed";
	error_codes[413] = "Request Entity Too Large";
	error_codes[414] = "Request-URI Too Long";
	error_codes[415] = "Unsupported Media Type";
	error_codes[416] = "Requested range unsatisfiable";
	error_codes[417] = "Expectation failed";
	error_codes[418] = "I’m a teapot";
	error_codes[419] = "Page expired";
	error_codes[421] = "Bad mapping / Misdirected Request";
	error_codes[422] = "Unprocessable entity";
	error_codes[423] = "Locked";
	error_codes[424] = "Method failure";
	error_codes[425] = "Too Early";
	error_codes[426] = "Upgrade Required";
	error_codes[427] = "Invalid digital signature";
	error_codes[428] = "Precondition Required";
	error_codes[429] = "Too Many Requests";
	error_codes[431] = "Request Header Fields Too Large";
	error_codes[449] = "Retry With";
	error_codes[450] = "Blocked by Windows Parental Controls";
	error_codes[451] = "Unavailable For Legal Reasons";
	error_codes[456] = "Unrecoverable Error";
	error_codes[444] = "No Response";
	error_codes[495] = "SSL Certificate Error";
	error_codes[496] = "SSL Certificate Required";
	error_codes[497] = "HTTP Request Sent to HTTPS Port";
	error_codes[498] = "Token expired/invalid";
	error_codes[499] = "Client Closed Request";
	error_codes[500] = "Internal Server Error";
	error_codes[501] = "Not Implemented";
	error_codes[502] = "Bad Gateway ou Proxy Error";
	error_codes[503] = "Service Unavailable";
	error_codes[504] = "Gateway Time-out";
	error_codes[505] = "HTTP Version not supported";
	error_codes[506] = "Variant Also Negotiates";
	error_codes[507] = "Insufficient storage";
	error_codes[508] = "Loop detected";
	error_codes[509] = "Bandwidth Limit Exceeded";
	error_codes[510] = "Not extended";
	error_codes[511] = "Network authentication required";
	error_codes[520] = "Unknown Error";
	error_codes[521] = "Web Server Is Down";
	error_codes[522] = "Connection Timed Out";
	error_codes[523] = "Origin Is Unreachable";
	error_codes[524] = "A Timeout Occurred";
	error_codes[525] = "SSL Handshake Failed";
	error_codes[526] = "Invalid SSL Certificate";
	error_codes[527] = "Railgun Error";

	generic_page << "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta charset=\"UTF-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t<title>"
		<< this->_error_code << " " << error_codes[this->_error_code] << "</title>\n\t</head>\n\t<body>\n\t\t<center><h1>"
		<< this->_error_code << " " << error_codes[this->_error_code] << "</h1></center>\n\t\t<hr><center>webserv</center>\n\t</body>\n</html>";

	return (generic_page.str());
}

void ServerResponse::process()
{
	std::string content;
	if (this->_error_code)	//	check value?	//	Make function to return error codes out of this scope's code
	{
		if (this->_error_pages.find(this->_error_code) != this->_error_pages.end())
			content = readFileContent(this->_root + this->_error_pages[this->_error_code]);
		else
			content = _getGenericErrorPage();
		sendHttpResponse(this->_client_socket, content, "text/html");	//	@TODO	send proper status code
		return;
	}
	switch(this->_method)
	{
	case GET:
	{
std::cout << "DEBUG:\n\troot:\t" << this->_root << "\n\tpath:\t" << this->_path << "\n\tindex:\t" << this->_index << std::endl;
		std::string const	locationPath = this->_root + this->_path;
		std::string			indexPath;
std::cout << "DEBUG: locationPath: " << locationPath << std::endl;

		if	(!access(locationPath.c_str(), F_OK))
		{
std::cout << "DEBUG: F_OK " << std::endl;
			struct stat		locationPathStat;

			stat(locationPath.c_str(), &locationPathStat);
			if (S_ISREG(locationPathStat.st_mode))	//	What if no perms to open file ?
			{
				if (access(locationPath.c_str(), R_OK))	//	Can happen ?
				{
					//	return 403
std::cout << "DEBUG: Permission denied 403" << std::endl;
					return ;
				}
				content = readFileContent(locationPath);
				sendHttpResponse(this->_client_socket, content, "text/html");
				break ;
			}
			if (S_ISDIR(locationPathStat.st_mode))	//	Check perms here too
			{
std::cout << "DEBUG: locationPath is a folder..." << std::endl;
				if (!(this->_redirect.empty()))
				{
					//	respond with redirection
					return ;
				}
				if (!(this->_cgi_path.empty()))
				{
					//	HANDLE CGI HERE
					return ;
				}
				indexPath = locationPath + "/" + this->_index;
				std::cout << "indexPath: " << indexPath << std::endl;
				if (!access(indexPath.c_str(), F_OK))
				{
std::cout << "DEBUG: Found index at: " << indexPath << std::endl;
					content = readFileContent(indexPath);
					sendHttpResponse(this->_client_socket, content, "text/html");
					break ;
				}
				if (this->_autoindex)
				{
					if (access(locationPath.c_str(), R_OK))
					{
std::cout << "DEBUG: Permission denied 403" << std::endl;
						//	return 403
						return ;
					}
					//	generate index.html, listing folder elements, and respond with generated file
						//	open locationPath dir and list elements
						DIR		*dirStream;;

						if (!(dirStream = opendir(locationPath.c_str())))
						{
							//	couldn't get directory stream
							//	return 5xx error
							break ;
						}

						std::stringstream	autoIndexedPage;

						//	generate file 'header'
						autoIndexedPage << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"UTF-8\">\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>"
						//	print folder name in title
							<< this->_path << "</title>\n\t<link rel=favicon ... >\n</head>\n<body>\n";	//	keep basename only in title

						//	loop on files
						struct dirent	*dirContent = NULL;
						do
						{
							errno = 0;
							if (!(dirContent = readdir(dirStream)) && errno)
							{
								//	readdir error
								//	return 5xx error
								return ;
							}
							if (!dirContent)
								break ;
							//	else if (!dirent)
								//empty folder
							//	else
								//	generate link
							std::string		fileName(dirContent->d_name);
							std::string		locationBasePath = locationPath.substr((this->_root.size()));	//	REMOVE TRAILING '/'s !
							autoIndexedPage << "\t<a href=\"" << (locationBasePath + "/" + fileName) << "\">" << fileName << "</a><br>\n";

						}
						while (dirContent != NULL);
						if (closedir(dirStream))
						{
							//	Error closing dir
							//	Send 5xx response
							return ;
						}

						//	generate file 'footer'
						autoIndexedPage << "</body>\n</html>\n";

						//	respond with generated html page
						sendHttpResponse(this->_client_socket, autoIndexedPage.str(), "text/html");
					return ;
				}
				//	return 404 error
std::cout << "DEBUG: Not found 404" << std::endl;
			}
		}
		else
		{
std::cout << "DEBUG: F_NOK " << std::endl;
			//	Respond with error number ...
std::cout << "DEBUG: Not found 404" << std::endl;
		}
		break ;
	}
	case POST:
		// Handle POST request
		// Generate appropriate response
		break ;
	case DELETE:
		// Handle DELETE request
		// Generate appropriate response
		break ;
	default:
		std::cerr << "CRITIC ERROR: Unauthorized method not catched in ServerResponse!" << std::endl;
	}
}

std::string ServerResponse::readFileContent(const std::string& filePath) {
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		perror("In opening file"); // @TODO return 404/5xx ?
		//exit(EXIT_FAILURE); // @TODO return 404/5xx ?
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ServerResponse::sendHttpResponse(int clientSocket, const std::string& content, const std::string& contentType) {	//	@TODO refactor: send proper return code + correct MIME type + use send
	std::stringstream httpHeaders;
	httpHeaders << "HTTP/1.1 200 OK\r\n" <<
				"Content-Length: " << content.size() << "\r\n" <<
				"Content-Type: " << contentType << "\r\n" <<
				"\r\n";
	std::string httpResponse = httpHeaders.str() + content;
	write(clientSocket, httpResponse.c_str(), httpResponse.size());
}
