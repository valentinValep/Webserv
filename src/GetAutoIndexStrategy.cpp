#include "GetAutoIndexStrategy.hpp"

GetAutoIndexStrategy::GetAutoIndexStrategy(ResponseBuildState * state): ResponseBuildingStrategy(state)
{}

GetAutoIndexStrategy::~GetAutoIndexStrategy() {}

void GetAutoIndexStrategy::buildResponse()
{
	// @TODO
	this->setResponse("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Autoindex</h1></body></html>");
	this->setAsFinished();
}
