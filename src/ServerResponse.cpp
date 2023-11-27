/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:53:57 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/27 14:51:43 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"
#include "ServerManager.hpp"
#include "CgiRequest.hpp"
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

std::string		ServerResponse::_getGenericErrorPage(int errorCode) const
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
		<< errorCode << " " << error_codes[errorCode] << "</title>\n\t</head>\n\t<body>\n\t\t<center><h1>"
		<< errorCode << " " << error_codes[errorCode] << "</h1></center>\n\t\t<hr><center>webserv</center>\n\t</body>\n</html>";
	return (generic_page.str());
}


void	ServerResponse::_sendErrorPage(int errorCode)	//	chec for correct value ?
{
	std::string		content;
	std::string		mimeType;

	if (this->_error_pages.find(errorCode) != this->_error_pages.end())
	{
		content = readFileContent(this->_root + "/" + this->_error_pages[errorCode], mimeType);
		if (content.empty())
			content = _getGenericErrorPage(errorCode);
	}
	else
		content = _getGenericErrorPage(errorCode);
	sendHttpResponse(errorCode, content, "text/html");
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
		if (this->_cgi_request)
		{
			CgiRequest cgiRequest(*this);
			// cgiRequest.printResponse();
			content = cgiRequest.getResponse();
			sendCGIResponse(this->_client_socket, content, "text/html");
			return ;
		}
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
		if (this->_file_upload)
		{
			createWriteFile();
			std::string mimeTypehere = "text/html";
			std::string content = readFileContent(this->_root + "/204.html", mimeTypehere);
			sendUploadResponse(this->_client_socket, content, "text/html");
			return ;
		}

		if (this->_cgi_request)
		{
			CgiRequest cgiRequest(*this);
			// cgiRequest.printResponse();
			content = cgiRequest.getResponse();
			sendCGIResponse(this->_client_socket, content, "text/html");
			return ;
		}
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
 *						UPLOAD								*
 ************************************************************/

int ServerResponse::createWriteFile() {
	int status = EXIT_SUCCESS;

	for (std::map<std::string, std::string>::iterator it = _upload_file_data.begin(); it != _upload_file_data.end(); ++it) {
		std::string filename = it->first;
		std::string fileContent = it->second;
		std::string path = _root + "/" + _upload_path + filename;

		std::ifstream file(path.c_str());
		if (file) {
			std::cout << "Warning: A file with the same name (" << filename << ") already exists and will be overwritten." << std::endl;
			file.close();
		}
		std::ofstream newFile(path.c_str());
		if (!newFile) {
			status = EXIT_FAILURE;
			std::cout << "ERROR: Failed to create file: " << filename << std::endl;
			continue;
		}
		newFile.write(fileContent.c_str(), fileContent.size());
		newFile.close();
	}
	return (status);
}

std::string ServerResponse::extractFileBody(size_t filenameEndPos) {
	std::string content = "";
	size_t fileStartPos = _body.find("\r\n\r\n", filenameEndPos) + 4;
	size_t fileEndPos = _body.find(_boundary, fileStartPos);
	if (fileStartPos < fileEndPos) {
		content = _body.substr(fileStartPos, fileEndPos - fileStartPos - 2);
	}
	return (content);
}

std::string ServerResponse::extractBoundary() {
	std::string boundary = "";
	std::string boundaryPrefix = "boundary=";
	std::map<std::string, std::string>::iterator it = _headers.find("Content-Type");

	if (it != _headers.end()) {
		size_t boundaryPos = it->second.find(boundaryPrefix);
		if (boundaryPos != std::string::npos) {
			boundaryPos += boundaryPrefix.length();
			boundary = it->second.substr(boundaryPos);
		}
		boundary.erase(std::remove(boundary.begin(), boundary.end(), '\r'), boundary.end());
		boundary.erase(std::remove(boundary.begin(), boundary.end(), '\n'), boundary.end());
	}
	return (boundary);
}

void ServerResponse::setUpload() {
	std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Type");

	if (it != this->_headers.end() && it->second.find("multipart/form-data") != std::string::npos) {

		size_t pos = 0;
		while ((pos = _body.find("filename=\"", pos)) != std::string::npos) {
			size_t filenamePos = pos;
			size_t filenameEndPos = _body.find("\"", filenamePos + 10);
			pos = filenameEndPos;
			if (filenameEndPos != std::string::npos && filenameEndPos > filenamePos + 10) {
				if (_file_upload == false)
				{
					this->_file_upload = true;
					this->_boundary = extractBoundary();
				}
				std::string filename = _body.substr(filenamePos + 10, filenameEndPos - (filenamePos + 10));
				std::string fileContent = extractFileBody(filenameEndPos);
				if (fileContent.size() > MAX_FILE_SIZE)
				{
					std::cout << "ERROR: File too large will not be uploaded: " << filename << std::endl;
					_sendErrorPage(413);
					continue;
				}
				_upload_file_data[filename] = fileContent;
			}
		}
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
