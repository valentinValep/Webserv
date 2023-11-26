#include "GetStrategy.hpp"
#include <iostream>

GetStrategy::GetStrategy(ResponseBuildState *state, bool is_autoindex, std::string index_file): ResponseBuildingStrategy(state), _is_autoindex(is_autoindex), _index_file(index_file)
{}

GetStrategy::~GetStrategy()
{}

void GetStrategy::buildResponse()
{
	(void)_is_autoindex;
	(void)_index_file;
	std::cout << "GetStrategy::buildResponse()" << std::endl;
}
