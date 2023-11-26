#include "DeleteStrategy.hpp"
#include <iostream>

DeleteStrategy::DeleteStrategy(ResponseBuildState *state): ResponseBuildingStrategy(state)
{}

DeleteStrategy::~DeleteStrategy()
{}

void DeleteStrategy::buildResponse()
{
	std::cout << "DeleteStrategy::buildResponse()" << std::endl;
}
