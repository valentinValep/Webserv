// FileName: Response.hpp

#pragma once

#include <string>
#include <map>

class Response {
private:
	// Attributes
	int									_code;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
public:
	// Constructors & Destructor
	Response();
	~Response();
	// Methods
	void		setCode(int code);
	void		addHeader(std::string key, std::string value);
	void		setBody(std::string body);

	std::string	build();
};