#include "ErrorStrategy.hpp"
#include "Response.hpp"
#include <iostream>
#include <fstream>

const std::map<int, std::string>	ErrorStrategy::_error_descriptions = ErrorStrategy::_createErrorDescriptionsMap();

const std::map<int, std::string> ErrorStrategy::_createErrorDescriptionsMap()
{
	std::map<int, std::string>	error_descriptions;

	error_descriptions[400] = "Bad Request";
	error_descriptions[401] = "Unauthorized";
	error_descriptions[402] = "Payment Required";
	error_descriptions[403] = "Forbidden";
	error_descriptions[404] = "Not Found";
	error_descriptions[405] = "Method Not Allowed";
	error_descriptions[406] = "Not Acceptable";
	error_descriptions[407] = "Proxy Authentication Required";
	error_descriptions[408] = "Request Time-out";
	error_descriptions[409] = "Conflict";
	error_descriptions[410] = "Gone";
	error_descriptions[411] = "Length Required";
	error_descriptions[412] = "Precondition Failed";
	error_descriptions[413] = "Request Entity Too Large";
	error_descriptions[414] = "Request-URI Too Long";
	error_descriptions[415] = "Unsupported Media Type";
	error_descriptions[416] = "Requested range unsatisfiable";
	error_descriptions[417] = "Expectation failed";
	error_descriptions[418] = "I’m a teapot";
	error_descriptions[419] = "Page expired";
	error_descriptions[421] = "Bad mapping / Misdirected Request";
	error_descriptions[422] = "Unprocessable entity";
	error_descriptions[423] = "Locked";
	error_descriptions[424] = "Method failure";
	error_descriptions[425] = "Too Early";
	error_descriptions[426] = "Upgrade Required";
	error_descriptions[427] = "Invalid digital signature";
	error_descriptions[428] = "Precondition Required";
	error_descriptions[429] = "Too Many Requests";
	error_descriptions[431] = "Request Header Fields Too Large";
	error_descriptions[449] = "Retry With";
	error_descriptions[450] = "Blocked by Windows Parental Controls";
	error_descriptions[451] = "Unavailable For Legal Reasons";
	error_descriptions[456] = "Unrecoverable Error";
	error_descriptions[444] = "No Response";
	error_descriptions[495] = "SSL Certificate Error";
	error_descriptions[496] = "SSL Certificate Required";
	error_descriptions[497] = "HTTP Request Sent to HTTPS Port";
	error_descriptions[498] = "Token expired/invalid";
	error_descriptions[499] = "Client Closed Request";
	error_descriptions[500] = "Internal Server Error";
	error_descriptions[501] = "Not Implemented";
	error_descriptions[502] = "Bad Gateway or Proxy Error";
	error_descriptions[503] = "Service Unavailable";
	error_descriptions[504] = "Gateway Time-out";
	error_descriptions[505] = "HTTP Version not supported";
	error_descriptions[506] = "Variant Also Negotiates";
	error_descriptions[507] = "Insufficient storage";
	error_descriptions[508] = "Loop detected";
	error_descriptions[509] = "Bandwidth Limit Exceeded";
	error_descriptions[510] = "Not extended";
	error_descriptions[511] = "Network authentication required";
	error_descriptions[520] = "Unknown Error";
	error_descriptions[521] = "Web Server Is Down";
	error_descriptions[522] = "Connection Timed Out";
	error_descriptions[523] = "Origin Is Unreachable";
	error_descriptions[524] = "A Timeout Occurred";
	error_descriptions[525] = "SSL Handshake Failed";
	error_descriptions[526] = "Invalid SSL Certificate";
	error_descriptions[527] = "Railgun Error";

	return (error_descriptions);
}

ErrorStrategy::ErrorStrategy(ResponseBuildState *state, int error_code, std::map<int, std::string> error_pages) : ResponseBuildingStrategy(state), _error_code(error_code), _error_pages(error_pages)
{}

ErrorStrategy::~ErrorStrategy()
{}

std::string		ErrorStrategy::_getGenericErrorPage()
{
	std::stringstream	generic_page;

	generic_page << "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta charset=\"UTF-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t<title>"
		<< this->_error_code << " " << ErrorStrategy::_error_descriptions.at(this->_error_code) << "</title>\n\t</head>\n\t<body>\n\t\t<center><h1>"
		<< this->_error_code << " " << ErrorStrategy::_error_descriptions.at(this->_error_code) << "</h1></center>\n\t\t<hr><center>webserv</center>\n\t</body>\n</html>";
	std::cout << generic_page.str() << std::endl;
	return (generic_page.str());
}

std::string ErrorStrategy::_getErrorPage()
{
	if (!this->_file_reader.isInitialized())
		this->_file_reader = FileReader(this->getState()->getRoot() + "/" + this->_error_pages[this->_error_code]);
	if (!this->_file_reader.isOpen())
		return this->_getGenericErrorPage();
	this->_file_reader.readChunk();
	if (this->_file_reader.isTotallyRead())
		return this->_file_reader.extractFileContent();
	return "";
}

void ErrorStrategy::buildResponse()
{
	std::string		content;

	if (this->_error_pages.find(this->_error_code) != this->_error_pages.end())
		content = this->_getErrorPage();
	else
		content = this->_getGenericErrorPage();

	if (!content.empty())
	{
		Response	response;

		response.setCode(this->_error_code);
		response.addHeader("Content-Type", "text/html");
		response.setBody(content);
		this->setResponse(response);
		this->setAsFinished();
	}
}
