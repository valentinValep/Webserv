#include "RedirectStrategy.hpp"
#include "ResponseBuilder.hpp"
#include <iostream>

RedirectStrategy::RedirectStrategy(ResponseBuildState *state, int code, std::string destination): ResponseBuildingStrategy(state), _code(code), _destination(destination)
{}

RedirectStrategy::~RedirectStrategy()
{}

void RedirectStrategy::buildResponse()
{
	ResponseBuilder		builder;

	builder.setCode(this->_code);
	builder.addHeader("Location", this->_destination);
	std::cout << "RedirectStrategy::buildResponse()" << std::endl;
	this->setResponse(builder.build());
	this->setAsFinished();
}
