#include "UploadStrategy.hpp"
#include "ResponseBuilder.hpp"
#include <iostream>

UploadStrategy::UploadStrategy(ResponseBuildState *state, std::string upload_dir, std::string body, std::map<std::string, std::string>headers): ResponseBuildingStrategy(state), _upload_dir(upload_dir), _body(body), _headers(headers)
{}

UploadStrategy::~UploadStrategy()
{
}

void UploadStrategy::buildResponse()
{
	(void)_upload_dir;

	ResponseBuilder	builder;
	this->getState()->getHeaders() = _headers;
	setUpload();
	createWriteFile();
	if (this->getError() > 0)
		return;
	std::string mimeTypehere = "text/html";
	std::string content = readFileContent("./ressources/201.html", mimeTypehere);	
	builder.setCode(201);
	builder.addHeader("Content-Type", "text/html");
	builder.setBody(content);
	this->setResponse(builder.build());
	std::cout << "UploadStrategy::buildResponse()" << std::endl;
	this->setAsFinished();
}


/************************************************************
 *						UPLOAD								*
 ************************************************************/

int UploadStrategy::createWriteFile() {
	int status = EXIT_SUCCESS;

	for (std::map<std::string, std::string>::iterator it = _upload_file_data.begin(); it != _upload_file_data.end(); ++it) {
		std::string filename = it->first;
		std::string fileContent = it->second;
		std::string path = this->getState()->getRoot() + "/" + _upload_dir + filename;

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

std::string UploadStrategy::extractFileBody(size_t filenameEndPos) {
	std::string content = "";
	size_t fileStartPos = _body.find("\r\n\r\n", filenameEndPos) + 4;
	size_t fileEndPos = _body.find(_boundary, fileStartPos);
	if (fileStartPos < fileEndPos) {
		content = _body.substr(fileStartPos, fileEndPos - fileStartPos - 2);
	}
	return (content);
}

std::string UploadStrategy::extractBoundary() {
	std::string boundary = "";
	std::string boundaryPrefix = "boundary=";
	std::map<std::string, std::string>::iterator it = this->_headers.find("Content-Type");

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

void UploadStrategy::setUpload() {
	std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Type");
	
	_boundary = extractBoundary();
	if (it != this->_headers.end() && it->second.find("multipart/form-data") != std::string::npos) {

		size_t pos = 0;
		while ((pos = _body.find("filename=\"", pos)) != std::string::npos) {
			size_t filenamePos = pos;
			size_t filenameEndPos = _body.find("\"", filenamePos + 10);
			pos = filenameEndPos;
			if (filenameEndPos != std::string::npos && filenameEndPos > filenamePos + 10) {
				std::string filename = _body.substr(filenamePos + 10, filenameEndPos - (filenamePos + 10));
				std::string fileContent = extractFileBody(filenameEndPos);
				if (fileContent.size() > MAX_FILE_SIZE)
				{
					std::cout << "ERROR: File too large will not be uploaded: " << filename << std::endl;
					this->setError(413);
					continue;
				}
				_upload_file_data[filename] = fileContent;
			}
		}
	}
}

std::string		UploadStrategy::readFileContent(std::string const &filePath, std::string &mimeType)
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
		this->setError(404);
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
