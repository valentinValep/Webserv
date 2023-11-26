#include "RedirectStrategy.hpp"
#include <iostream>

RedirectStrategy::RedirectStrategy(ResponseBuildState *state, int code, std::string destination): ResponseBuildingStrategy(state), _code(code), _destination(destination)
{}

RedirectStrategy::~RedirectStrategy()
{}

void RedirectStrategy::buildResponse()
{
	(void)_code;
	(void)_destination;
	std::cout << "RedirectStrategy::buildResponse()" << std::endl;
}
