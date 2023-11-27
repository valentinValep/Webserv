#include "ErrorStrategy.hpp"
#include <iostream>

ErrorStrategy::ErrorStrategy(ResponseBuildState *state, int error_code, std::map<int, std::string> error_pages): ResponseBuildingStrategy(state), _error_code(error_code), _error_pages(error_pages)
{}

ErrorStrategy::~ErrorStrategy()
{}

void ErrorStrategy::buildResponse()
{
	(void)_error_pages;
	(void)_error_code;
	std::cout << "ErrorStrategy::buildResponse()" << std::endl;
	this->setAsFinished();
}
