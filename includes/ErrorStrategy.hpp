// FileName: ErrorStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
#include "FileReader.hpp"

class ErrorStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	static const std::map<int, std::string>	_error_descriptions;
	int							_error_code;
	std::map<int, std::string>	_error_pages;
	FileReader					_file_reader;

	std::string								_getErrorPage();
	std::string								_getGenericErrorPage();
	static const std::map<int, std::string>	_createErrorDescriptionsMap();
public:
	// Constructors & Destructor
	ErrorStrategy(ResponseBuildState* state, int error_code, std::map<int, std::string> error_pages);
	virtual ~ErrorStrategy();
	// Methods
	void	buildResponse();
};
