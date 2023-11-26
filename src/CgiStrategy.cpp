#include "CgiStrategy.hpp"
#include <iostream>

CgiStrategy::CgiStrategy(ResponseBuildState *state, std::string extension, std::string interpreter): ResponseBuildingStrategy(state), _extension(extension), _interpreter(interpreter)
{}

CgiStrategy::~CgiStrategy()
{
}

void CgiStrategy::buildResponse()
{
	(void)_extension;
	(void)_interpreter;
	std::cout << "CgiStrategy::buildResponse()" << std::endl;
}
