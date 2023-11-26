#include "ResponseBuildingStrategy.hpp"

ResponseBuildingStrategy::ResponseBuildingStrategy(ResponseBuildState* state): _error_code(0), _finished(false), _state(state)
{}

ResponseBuildingStrategy::~ResponseBuildingStrategy() {}

int ResponseBuildingStrategy::getError()
{
	return this->_error_code;
}

std::string ResponseBuildingStrategy::getResponse()
{
	return this->_response;
}

ResponseBuildState *ResponseBuildingStrategy::getState()
{
	return this->_state;
}

void ResponseBuildingStrategy::setError(int error_code)
{
	this->_error_code = error_code;
}

void ResponseBuildingStrategy::setAsFinished()
{
	this->_finished = true;
}

void ResponseBuildingStrategy::setResponse(std::string response)
{
	this->_response = response;
}

bool ResponseBuildingStrategy::isFinished()
{
	return this->_finished;
}


