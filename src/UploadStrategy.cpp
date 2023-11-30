#include "UploadStrategy.hpp"
#include "ResponseBuilder.hpp"
#include "FileReader.hpp"
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

	FileReader filereader = FileReader("./ressources/201.html");
	if (!filereader.isOpen())
	{
		this->setError(500);
		return;
	}
	filereader.readFile();
	builder.setCode(201);
	builder.addHeader("Content-Type", "text/html");
	builder.setBody(filereader.extractFileContent());
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
			this->setError(404);
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
