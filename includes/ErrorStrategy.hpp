// FileName: ErrorStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class ErrorStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	int							_error_code;
	std::map<int, std::string>	_error_pages;
public:
	// Constructors & Destructor
	ErrorStrategy(ResponseBuildState* state, int error_code, std::map<int, std::string> error_pages);
	virtual ~ErrorStrategy();
	// Methods
	void	buildResponse();
};
