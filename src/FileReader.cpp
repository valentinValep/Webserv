#include "FileReader.hpp"

const std::map<std::string, std::string>	FileReader::createMimeTypesMap()
{
	std::map<std::string, std::string>	mime_types;

	mime_types[".aac"] = "audio/aac";
	mime_types[".abw"] = "application/x-abiword";
	mime_types[".arc"] = "application/x-freearc";
	mime_types[".avif"] = "image/avif";
	mime_types[".avi"] = "video/x-msvideo";
	mime_types[".azw"] = "application/vnd.amazon.ebook";
	mime_types[".bin"] = "application/octet-stream";
	mime_types[".bmp"] = "image/bmp";
	mime_types[".bz"] = "application/x-bzip";
	mime_types[".bz2"] = "application/x-bzip2";
	mime_types[".cda"] = "application/x-cdf";
	mime_types[".csh"] = "application/x-csh";
	mime_types[".css"] = "text/css";
	mime_types[".csv"] = "text/csv";
	mime_types[".doc"] = "application/msword";
	mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_types[".eot"] = "application/vnd.ms-fontobject";
	mime_types[".epub"] = "application/epub+zip";
	mime_types[".gz"] = "application/gzip";
	mime_types[".gif"] = "image/gif";
	mime_types[".html"] = "text/html";
	mime_types[".htm"] = "text/html";
	mime_types[".ico"] = "image/vnd.microsoft.icon";
	mime_types[".ics"] = "text/calendar";
	mime_types[".jar"] = "application/java-archive";
	mime_types[".jpg"] = "image/jpeg";
	mime_types[".jpeg"] = "image/jpeg";
	mime_types[".js"] = "text/javascript";
	mime_types[".json"] = "application/json";
	mime_types[".jsonld"] = "application/ld+json";
	mime_types[".midi"] = "audio/x-midi";
	mime_types[".midi"] = "audio/midi";
	mime_types[".mid"] = "audio/x-midi";
	mime_types[".mid"] = "audio/midi";
	mime_types[".mjs"] = "text/javascript";
	mime_types[".mp3"] = "audio/mpeg";
	mime_types[".mp4"] = "video/mp4";
	mime_types[".mpeg"] = "video/mpeg";
	mime_types[".mpkg"] = "application/vnd.apple.installer+xml";
	mime_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mime_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mime_types[".odt"] = "application/vnd.oasis.opendocument.text";
	mime_types[".oga"] = "audio/ogg";
	mime_types[".ogv"] = "video/ogg";
	mime_types[".ogx"] = "application/ogg";
	mime_types[".opus"] = "audio/opus";
	mime_types[".otf"] = "font/otf";
	mime_types[".png"] = "image/png";
	mime_types[".pdf"] = "application/pdf";
	mime_types[".php"] = "application/x-httpd-php";
	mime_types[".ppt"] = "application/vnd.ms-powerpoint";
	mime_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_types[".rar"] = "application/vnd.rar";
	mime_types[".rtf"] = "application/rtf";
	mime_types[".sh"] = "application/x-sh";
	mime_types[".svg"] = "image/svg+xml";
	mime_types[".tar"] = "application/x-tar";
	mime_types[".tiff"] = "image/tiff";
	mime_types[".tif"] = "image/tiff";
	mime_types[".ts"] = "video/mp2t";
	mime_types[".ttf"] = "font/ttf";
	mime_types[".txt"] = "text/plain";
	mime_types[".vsd"] = "application/vnd.visio";
	mime_types[".wav"] = "audio/wav";
	mime_types[".weba"] = "audio/webm";
	mime_types[".webm"] = "video/webm";
	mime_types[".webp"] = "image/webp";
	mime_types[".woff"] = "font/woff";
	mime_types[".woff2"] = "font/woff2";
	mime_types[".xhtml"] = "application/xhtml+xml";
	mime_types[".xls"] = "application/vnd.ms-excel";
	mime_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_types[".xml"] = "application/xml";
	mime_types[".xul"] = "application/vnd.mozilla.xul+xml";
	mime_types[".zip"] = "application/zip";
	mime_types[".3gp"] = "video/3gpp";
	mime_types[".3g2"] = "video/3gpp2";
	mime_types[".7z"] = "application/x-7z-compressed";
	return mime_types;
}

const std::map<std::string, std::string>	FileReader::_mime_types = createMimeTypesMap();

FileReader::FileReader() {}

FileReader::FileReader(std::string file_path)
{
	this->_file_path = file_path;
	this->_file_stream.open(this->_file_path.c_str());
}

FileReader::FileReader(const FileReader &cpy): _file_path(cpy._file_path), _file_content(cpy._file_content)
{
	this->_file_stream.open(this->_file_path.c_str());
}

FileReader &FileReader::operator=(const FileReader &cpy)
{
	if (this == &cpy)
		return *this;
	this->_file_path = cpy._file_path;
	this->_file_content = cpy._file_content;
	this->_file_stream.open(this->_file_path.c_str());
	return *this;
}

FileReader::~FileReader()
{
	this->_file_stream.close();
}

std::string FileReader::extractFileContent()
{
	std::string		cpy = this->_file_content;

	this->_file_content.clear();
	return cpy;
}

std::string FileReader::getFileContent()
{
	return this->_file_content;
}

std::string FileReader::getMimeType()
{
	std::size_t pos = this->_file_path.find_last_of('.');

	if (pos == std::string::npos)
		return "";
	return this->_mime_types.find(this->_file_path.substr(pos))->second;
}

void FileReader::readFile()
{
	this->_file_stream.seekg(0, std::ios::end);
	this->_file_content.reserve(this->_file_stream.tellg());
	this->_file_stream.seekg(0, std::ios::beg);
	this->_file_content.assign((std::istreambuf_iterator<char>(this->_file_stream)), std::istreambuf_iterator<char>());
}

void FileReader::readChunk()
{
	char	buffer[FILE_READER_BUFFER_SIZE];

	this->_file_stream.read(buffer, FILE_READER_BUFFER_SIZE);
	this->_file_content.append(buffer, this->_file_stream.gcount());
}

bool FileReader::isTotallyRead()
{
	return this->_file_stream.eof();
}

bool FileReader::isOpen()
{
	return this->_file_stream.is_open();
}

bool FileReader::isInitialized()
{
	return !(this->_file_path.empty());
}
