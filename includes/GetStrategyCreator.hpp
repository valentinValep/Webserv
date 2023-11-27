// FileName: GetStrategyCreator.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class GetStrategyCreator {
public:
	static ResponseBuildingStrategy		*createGetStrategy(ResponseBuildState *state, bool is_autoindex, std::string index_file);
};
