/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/20 17:21:41 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include "CgiRequest.hpp"
#include "Route.hpp"
#include <sstream>

ServerResponse::ServerResponse() : _port(0), _autoindex(false), _error_code(0), _method(0), _redirect_type(0), _cgi_request(false)
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

	this->_port = request.getPort();

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
	if (request.getProtocol() != HTTP_PROTOCOL) // @TODO move to ClientRequest ?
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
		if (route->hasCgi() || request.isCgiRequest()) // @TODO remove the check from request ?
		{
			this->_cgi_request = true;
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

std::string		trimTrailingSlashes(std::string &path)
{
	size_t	index;

	if (!path.empty())
	{
//		if ((index = path.find_last_of("/")) != path.npos)
//			if (!(path[index + 1]))
//				path.erase(index);
		if ((index = path.find_last_not_of("/")) != path.npos)
			path.erase(index + 1);
	}
	return (path);
}

// @TODO if _error_code is 400, send error and close connection maybe not for all 400 (check RFC last paragraph section 2.2)
void ServerResponse::process()
{
	std::string		content;
	std::string		mimeType;

	if (this->_error_code)	//	check value?	//	Make function to return error codes out of this scope's code
	{
		if (this->_error_pages.find(this->_error_code) != this->_error_pages.end())
			content = readFileContent(this->_root + this->_error_pages[this->_error_code], mimeType);
		else
			content = _getGenericErrorPage();
		sendHttpResponse(this->_error_code, content, "text/html");	//	@TODO	send proper status code
		return ;
	}
	if (this->_cgi_request)
	{
		std::cout << "CGI FOUND" << std::endl;
		CgiRequest cgiRequest(*this);
		std::cout << " THE REPONSE IS " << std::endl;
		cgiRequest.printResponse();
		content = cgiRequest.getResponse();
		sendCGIResponse(this->_client_socket, content, "text/html");
		return ;
	}
	switch(this->_method)
	{
	case GET:
	{
std::cout << "DEBUG:\n\troot:\t" << this->_root << "\n\tpath:\t" << this->_path << "\n\tindex:\t" << this->_index << std::endl;
		std::string const	locationPath = this->_root + trimTrailingSlashes(this->_path);
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
				content = readFileContent(locationPath, mimeType);
				sendHttpResponse(200, content, mimeType);	//	search MIME type
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
					content = readFileContent(indexPath, mimeType);
					sendHttpResponse(200, content, mimeType);	//	Should be of type html here. BUT what if our index page is of type .php or something else ?
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
							<< this->_path << "</title>\n\t<link rel=favicon ... >\n</head>\n<body>\n";	//	keep basename only in title	//	Add "content of "folder"' in html page

						//	loop on files
						struct dirent	*dirContent = NULL;
						do	//	make infinite loop
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
						sendHttpResponse(200, autoIndexedPage.str(), "text/html");
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

std::string		ServerResponse::readFileContent(std::string const &filePath, std::string &mimeType)
{
	std::ifstream								file(filePath.c_str(), std::ios::in | std::ios::binary);
	static std::map<std::string, std::string>	mimeTypeList;
	std::string									extension;
	size_t										extensionIndex;

	mimeTypeList[".aac"] = "audio/aac";
	mimeTypeList[".abw"] = "application/x-abiword";
	mimeTypeList[".arc"] = "application/x-freearc";
	mimeTypeList[".avif"] = "image/avif";
	mimeTypeList[".avi"] = "video/x-msvideo";
	mimeTypeList[".azw"] = "application/vnd.amazon.ebook";
	mimeTypeList[".bin"] = "application/octet-stream";
	mimeTypeList[".bmp"] = "image/bmp";
	mimeTypeList[".bz"] = "application/x-bzip";
	mimeTypeList[".bz2"] = "application/x-bzip2";
	mimeTypeList[".cda"] = "application/x-cdf";
	mimeTypeList[".csh"] = "application/x-csh";
	mimeTypeList[".css"] = "text/css";
	mimeTypeList[".csv"] = "text/csv";
	mimeTypeList[".doc"] = "application/msword";
	mimeTypeList[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mimeTypeList[".eot"] = "application/vnd.ms-fontobject";
	mimeTypeList[".epub"] = "application/epub+zip";
	mimeTypeList[".gz"] = "application/gzip";
	mimeTypeList[".gif"] = "image/gif";
	mimeTypeList[".html"] = "text/html";
	mimeTypeList[".htm"] = "text/html";
	mimeTypeList[".ico"] = "image/vnd.microsoft.icon";
	mimeTypeList[".ics"] = "text/calendar";
	mimeTypeList[".jar"] = "application/java-archive";
	mimeTypeList[".jpg"] = "image/jpeg";
	mimeTypeList[".jpeg"] = "image/jpeg";
	mimeTypeList[".js"] = "text/javascript";
	mimeTypeList[".json"] = "application/json";
	mimeTypeList[".jsonld"] = "application/ld+json";
	mimeTypeList[".midi"] = "audio/x-midi";
	mimeTypeList[".midi"] = "audio/midi";
	mimeTypeList[".mid"] = "audio/x-midi";
	mimeTypeList[".mid"] = "audio/midi";
	mimeTypeList[".mjs"] = "text/javascript";
	mimeTypeList[".mp3"] = "audio/mpeg";
	mimeTypeList[".mp4"] = "video/mp4";
	mimeTypeList[".mpeg"] = "video/mpeg";
	mimeTypeList[".mpkg"] = "application/vnd.apple.installer+xml";
	mimeTypeList[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mimeTypeList[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mimeTypeList[".odt"] = "application/vnd.oasis.opendocument.text";
	mimeTypeList[".oga"] = "audio/ogg";
	mimeTypeList[".ogv"] = "video/ogg";
	mimeTypeList[".ogx"] = "application/ogg";
	mimeTypeList[".opus"] = "audio/opus";
	mimeTypeList[".otf"] = "font/otf";
	mimeTypeList[".png"] = "image/png";
	mimeTypeList[".pdf"] = "application/pdf";
	mimeTypeList[".php"] = "application/x-httpd-php";
	mimeTypeList[".ppt"] = "application/vnd.ms-powerpoint";
	mimeTypeList[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mimeTypeList[".rar"] = "application/vnd.rar";
	mimeTypeList[".rtf"] = "application/rtf";
	mimeTypeList[".sh"] = "application/x-sh";
	mimeTypeList[".svg"] = "image/svg+xml";
	mimeTypeList[".tar"] = "application/x-tar";
	mimeTypeList[".tiff"] = "image/tiff";
	mimeTypeList[".tif"] = "image/tiff";
	mimeTypeList[".ts"] = "video/mp2t";
	mimeTypeList[".ttf"] = "font/ttf";
	mimeTypeList[".txt"] = "text/plain";
	mimeTypeList[".vsd"] = "application/vnd.visio";
	mimeTypeList[".wav"] = "audio/wav";
	mimeTypeList[".weba"] = "audio/webm";
	mimeTypeList[".webm"] = "video/webm";
	mimeTypeList[".webp"] = "image/webp";
	mimeTypeList[".woff"] = "font/woff";
	mimeTypeList[".woff2"] = "font/woff2";
	mimeTypeList[".xhtml"] = "application/xhtml+xml";
	mimeTypeList[".xls"] = "application/vnd.ms-excel";
	mimeTypeList[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mimeTypeList[".xml"] = "application/xml";
	mimeTypeList[".xul"] = "application/vnd.mozilla.xul+xml";
	mimeTypeList[".zip"] = "application/zip";
	mimeTypeList[".3gp"] = "video/3gpp";
	mimeTypeList[".3g2"] = "video/3gpp2";
	mimeTypeList[".7z"] = "application/x-7z-compressed";

	if (!file.is_open()) {
		perror("In opening file"); // @TODO return 404/5xx ?
		//exit(EXIT_FAILURE); // @TODO return 404/5xx ?
	}
	//	determin MIME type here
		mimeType = "";
		//	find extension
		if ((extensionIndex = filePath.find_last_of(".")) != filePath.npos)
		{
		//	compare extension (what if no extension ?)
			if (mimeTypeList.find(filePath.substr(extensionIndex)) != mimeTypeList.end())	//	What if not found ?
				mimeType = mimeTypeList[filePath.substr(extensionIndex)];
		}
	//	MimeType = determined MIME type here
		mimeType = mimeTypeList[filePath.substr(extensionIndex)];
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ServerResponse::sendHttpResponse(int const responseCode, std::string const &content, std::string const &contentType) {	//	@TODO refactor: send proper return code + correct MIME type + use send
	std::stringstream httpHeaders;
	httpHeaders << "HTTP/" << HTTPVERSION << " " << responseCode << " \r\n"
				<< "Content-Length: " << content.size() << "\r\n"
				<< (contentType.empty() ? "" : "Content-Type: ")
				<< (contentType.empty() ? "" : contentType) << "\r\n"
				<< "\r\n";
	std::string httpResponse = httpHeaders.str() + content;
	write(this->_client_socket, httpResponse.c_str(), httpResponse.size());
}

void ServerResponse::sendCGIResponse(int clientSocket, const std::string& content, const std::string& contentType) {
	std::stringstream cgiHeaders;
	cgiHeaders << "HTTP/1.1 200 OK\r\n" <<
				"Content-Length: " << content.size() << "\r\n" <<
				"Content-Type: " << contentType << "\r\n" <<
				"\r\n";
	std::string cgiResponse = cgiHeaders.str() + content;
	write(clientSocket, cgiResponse.c_str(), cgiResponse.size());
}

/************************************************************
 *						GETTERS								*
 ************************************************************/

int ServerResponse::getMethod() const
{
	return (this->_method);
}

int ServerResponse::getPort() const
{
	return (this->_port);
}

int	ServerResponse::getContentLength() const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Length");
	if (it != this->_headers.end())
		return (atoi(it->second.c_str()));
	return (0);
}

std::string	ServerResponse::getContentType() const
{
	std::string contentType = "";
	std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Type");
	if (it != this->_headers.end())
		contentType = it->second;
	return (contentType);
}

std::string ServerResponse::getPath() const
{
	return (this->_path);
}

std::string ServerResponse::getRoot() const
{
	return (this->_root);
}

std::string	ServerResponse::getCgiPath() const
{
	return (this->_cgi_path);
}