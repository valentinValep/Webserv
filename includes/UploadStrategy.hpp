// FileName: UploadStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <fstream>
# include <iostream>
# include <algorithm>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <sstream>
# include <cstdio>

# define MAX_FILE_SIZE 10 * 1024 * 1024 // 1MB

class UploadStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	std::string							_upload_dir;
	std::string							_body;
	std::string							_boundary;
	std::map<std::string, std::string>	_upload_file_data;
	std::map<std::string, std::string>	_headers;
public:
	// Constructors & Destructor
	UploadStrategy(ResponseBuildState* state, std::string upload_dir, std::string body, std::map<std::string, std::string>headers);
	virtual ~UploadStrategy();
	// Methods
	void	buildResponse();

	// Upload
		void			setUpload();
		std::string		extractBoundary();
		std::string		extractFileBody(size_t filenameEndPos);
		int				createWriteFile();
		std::string		readFileContent(std::string const &filePath, std::string &mimeType);
};