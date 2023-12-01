#include "RedirectStrategy.hpp"

RedirectStrategy::RedirectStrategy(ResponseBuildState *state, int code, std::string destination): ResponseBuildingStrategy(state), _code(code), _destination(destination)
{}

RedirectStrategy::~RedirectStrategy()
{}

void RedirectStrategy::buildResponse()
{
	Response		response;

	response.setCode(this->_code);
	response.addHeader("Location", this->_destination);
	std::cout << "RedirectStrategy::buildResponse()" << std::endl;
	this->setResponse(response);
	this->setAsFinished();
}
