// FileName: FileReader.hpp

#pragma once

#include <string>
#include <fstream>
#include <map>

#define FILE_READER_BUFFER_SIZE 4096

class FileReader {
private:
	// Attributes
	static const std::map<std::string, std::string>	_mime_types;
	std::string										_file_path;
	std::string										_file_content;
	std::ifstream									_file_stream;

	static const std::map<std::string, std::string>	createMimeTypesMap();
public:
	// Constructors & Destructor
	FileReader();
	FileReader(std::string file_path);
	FileReader(const FileReader &cpy);
	FileReader &operator=(const FileReader &cpy);
	~FileReader();
	// Methods
	std::string		extractFileContent();
	std::string		getFileContent();
	std::string		getMimeType();
	void			readFile();
	void			readChunk();
	bool			isTotallyRead();
	bool			isOpen();
	bool			isInitialized();
};