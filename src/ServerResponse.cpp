/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/27 19:31:44 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include "Route.hpp"

ServerResponse::ServerResponse() : _port(0), _autoindex(false), _error_code(0), _method(0), _redirect_type(0), _cgi_request(false), _file_upload(false)
{}

void ServerResponse::setError(int errorCode)
{
	this->_error_code = errorCode;
}

void ServerResponse::prepare(const ClientRequest &request)
{
	//if (!request.getServer())
	//	return this->setError(500);

	//const Route	*route = request.getServer()->getRoute(request.getPath());

	//this->_port = request.getPort();

	// Client socket
	this->_client_socket = request.getClientSocket();
	// Error pages for ERROR STRAT
	this->_error_pages = request.getServer()->getErrorPages();
	// Autoindex for INDEX STRAT
	if (route && route->hasAutoindex())
		this->_autoindex = route->getAutoindex();
	else
		this->_autoindex = request.getServer()->getAutoindex();
	// Root for ALL STRAT
	if (route && route->hasRoot())
		this->_root = route->getRoot();
	else
		this->_root = request.getServer()->getRoot();
	 Index for INDEX STRAT
	if (route && route->hasIndex())
		this->_index = route->getIndex();
	else if (route && request.getPath() == "/")
		this->_index = request.getServer()->getIndex();
	else if (route)
		this->_index = "";
	else
		this->_index = request.getServer()->getIndex();
	// Error
	if (request.getErrorCode())
		return this->setError(request.getErrorCode());

	//// Method
	//if (route && route->hasMethods())
	//	this->_method = route->getMethods() & request.getMethod();
	//else
	//	this->_method = request.getServer()->getMethods() & request.getMethod();
	//std::cout << "method: " << this->_method << std::endl;
	//if (!this->_method)
	//	return this->setError(405);
	//if (request.getProtocol() != HTTP_PROTOCOL && request.getProtocol() != "undefined") // @TODO move to ClientRequest ?
	//	return this->setError(505);
	//this->_path = request.getPath();
	//this->_headers = request.getHeaders();

	//if (route)
	//{
		//// Redirect
		//if (route->hasRedirect())
		//{
		//	this->_redirect_type = route->getRedirectCode();
		//	this->_redirect = route->getRedirectDest();
		//}
		// CGI
		//if (route->hasCgi() || request.isCgiRequest()) // @TODO remove the check from request ?
		//{
		//	this->_cgi_request = true;
		//	this->_cgi_extension = route->getCgiExtension();
		//	this->_cgi_path = route->getCgiPath();
		//}
		//// Upload
		//this->_body = request.getBodyBody();
		//setUpload();
		//if (this->_file_upload) {
		//	if (route->hasUpload()) {
		//		this->_upload_path = route->getUploadPath(); }
		//	else
		//	{
		//		std::cout << "Error: upload path has not been set, upload not allowed" << std::endl;
		//		_file_upload = false;
		//	}
		//}
	//}
}

void	ServerResponse::_sendAutoIndexed(std::string const &locationPath)
{
	DIR					*dirStream;;
	std::stringstream	autoIndexedPage;
	struct stat			fileStat;

	if (!(dirStream = opendir(locationPath.c_str())))
	{
		_sendErrorPage(500);
		return ;
	}
	autoIndexedPage << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"UTF-8\">\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>"
					<< this->_path << "</title>\n\t<link rel=\"stylesheet\" type=\"text/css\" href=\"/style/style.css\">\n\t<link rel=\"stylesheet\" type=\"text/css\" href=\"/style/autoindex.css\">\n\t"
					<< "<link rel=\"icon\" href=\"/favicon.gif\" type=\"image/gif\">\n</head>\n<body>\n\t<h1>Index of " << this->_path << "</h1>\n\t<hr>\n";

	struct dirent	*dirContent = NULL;

	while (42)
	{
		errno = 0;
		if (!(dirContent = readdir(dirStream)) && errno)
		{
			_sendErrorPage(500);
			return ;
		}
		if (!dirContent)
			break ;

		std::string		fileName(dirContent->d_name);

		if (stat((locationPath + "/" + fileName).c_str(), &fileStat))
		{
			_sendErrorPage(500);
			closedir(dirStream);
			return ;
		}
		autoIndexedPage << "  <a class=\"space" << (S_ISDIR(fileStat.st_mode) ? " dir" : (S_ISREG(fileStat.st_mode) ? " file" : ""))
			<< "\" href=\"" << (locationPath.substr((this->_root.size())) + "/" + fileName) << "\">" << fileName << "</a><br>\n";
	}
	if (closedir(dirStream))
	{
		_sendErrorPage(500);
		return ;
	}
	autoIndexedPage << "</body>\n</html>\n";
	sendHttpResponse(200, autoIndexedPage.str(), "text/html");
}

// @TODO if _error_code is 400, send error and close connection maybe not for all 400 (check RFC last paragraph section 2.2)
void ServerResponse::process()
{
	std::string		content;
	std::string		mimeType;

	if (this->_error_code)	//	check value ?
	{
		_sendErrorPage(this->_error_code);
		return ;
	}
	std::string const	locationPath = this->_root + trimTrailingSlashes(this->_path);
	switch(this->_method)
	{
	case GET:
	{
		// if (this->_cgi_request)
		// {
		// 	CgiRequest cgiRequest(*this);
		// 	// cgiRequest.printResponse();
		// 	content = cgiRequest.getResponse();
		// 	sendCGIResponse(this->_client_socket, content, "text/html");
		// 	return ;
		// }
		if (!(this->_redirect.empty()))
		{
			sendHttpRedirection();
			return ;
		}
		std::string			indexPath;

		if(!access(locationPath.c_str(), F_OK))
		{
			struct stat		locationPathStat;

			if(access(locationPath.c_str(), R_OK))
			{
				_sendErrorPage(403);
				return ;
			}
			if (stat(locationPath.c_str(), &locationPathStat))
			{
				_sendErrorPage(500);
				return ;
			}
			if (S_ISREG(locationPathStat.st_mode))
			{
				content = readFileContent(locationPath, mimeType);
				sendHttpResponse(200, content, mimeType);
				return ;
			}
			if (S_ISDIR(locationPathStat.st_mode))
			{
				if (!(this->_redirect.empty()))
				{
					sendHttpRedirection();
					return ;
				}
//				if (!(this->_cgi_path.empty()))	//	!	\\	Should be handled here too ??
//				{
//					//	HANDLE CGI HERE
//					return ;
//				}
				// @TODO check this->index not empty
				indexPath = locationPath + "/" + this->_index;
				if (!access(indexPath.c_str(), F_OK))
				{
					if (!access(indexPath.c_str(), R_OK))
					{
						content = readFileContent(indexPath, mimeType);
						sendHttpResponse(200, content, mimeType);
						return ;
					}
					_sendErrorPage(403);
					return ;
				}
				if (this->_autoindex)
				{
					_sendAutoIndexed(locationPath);
					return ;
				}
				_sendErrorPage(404);
			}
		}
		else
			_sendErrorPage(404);
		break ;
	}
	case POST:
	{
		// if (this->_file_upload)
		// {
		// 	createWriteFile();
		// 	std::string mimeTypehere = "text/html";
		// 	std::string content = readFileContent(this->_root + "/204.html", mimeTypehere);
		// 	sendUploadResponse(this->_client_socket, content, "text/html");
		// 	return ;
		// }

		// if (this->_cgi_request)
		// {
		// 	CgiRequest cgiRequest(*this);
		// 	// cgiRequest.printResponse();
		// 	content = cgiRequest.getResponse();
		// 	sendCGIResponse(this->_client_socket, content, "text/html");
		// 	return ;
		// }
		break ;
	}
	case DELETE:
		{
			if (!(access(locationPath.c_str(), F_OK)))
			{
				if (!(access(locationPath.c_str(), W_OK)))
				{
					//	if ok, delete file, respond with 204 or with 200 if a response html has to be sent
					std::remove(locationPath.c_str());
					sendHttpResponse(204, "", "text/html");
				}
				else
					_sendErrorPage(403);
			}
			_sendErrorPage(404);
			break ;
		}
	default:
		std::cerr << " \033[91mCRITIC Error\033[0m: Unauthorized method not catched in ServerResponse!" << std::endl;
	}
}

/************************************************************
 *						SEND RESPONSE						*
 ************************************************************/

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
		perror(("In opening file " + filePath).c_str()); // @TODO return 404/5xx ?
		//exit(EXIT_FAILURE); // @TODO return 404/5xx ?
	}
	mimeType = "";
	if ((extensionIndex = filePath.find_last_of(".")) != filePath.npos)
	{
		if (mimeTypeList.find(filePath.substr(extensionIndex)) != mimeTypeList.end())	//	What if not found ?
			mimeType = mimeTypeList[filePath.substr(extensionIndex)];
	}
	mimeType = mimeTypeList[filePath.substr(extensionIndex)];
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ServerResponse::sendHttpResponse(int const responseCode, std::string const &content, std::string const &contentType) {	//	@TODO refactor: use send()
	std::stringstream httpHeaders;
	httpHeaders << "HTTP/" << HTTPVERSION << " " << responseCode << " \r\n"
				<< "Content-Length: "
				<< (content.empty() ? 0 : content.size()) << "\r\n"
				<< (contentType.empty() ? "" : "Content-Type: ")
				<< (contentType.empty() ? "" : contentType) << "\r\n"
				<< (responseCode == 400 ? "Connection: close" : "") << "\r\n"
				<< "\r\n";
	std::string httpResponse = httpHeaders.str() + content;
	// @TODO handle broken pipe
	write(this->_client_socket, httpResponse.c_str(), httpResponse.size());
}

void ServerResponse::sendHttpRedirection() {	//	@TODO refactor: use send()
	std::stringstream httpHeaders;
	httpHeaders << "HTTP/" << HTTPVERSION << " " << this->_redirect_type << " \r\n"
				<< "Location: " << this->_redirect << "\r\n"
				<< "\r\n";
	write(this->_client_socket, httpHeaders.str().c_str(), httpHeaders.str().size());
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

void ServerResponse::sendUploadResponse(int clientSocket, const std::string& content, const std::string& contentType) {
	std::stringstream cgiHeaders;
	cgiHeaders << "HTTP/1.1 201 OK\r\n" <<
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
