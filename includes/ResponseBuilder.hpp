// FileName: ResponseBuilder.hpp

#pragma once

#include <string>
#include <map>

class ResponseBuilder {
private:
	// Attributes
	int									_code;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
public:
	// Constructors & Destructor
	ResponseBuilder();
	~ResponseBuilder();
	// Methods
	void		setCode(int code);
	void		addHeader(std::string key, std::string value);
	void		setBody(std::string body);

	std::string	build();
};