#include "GetFileStrategy.hpp"

GetFileStrategy::GetFileStrategy(ResponseBuildState *state, std::string file_path): ResponseBuildingStrategy(state), _file_path(file_path)
{}

GetFileStrategy::~GetFileStrategy() {}

void GetFileStrategy::buildResponse()
{
	this->setResponse("HTTP/1.1 200 OK\r\n");
	this->setAsFinished();
}
