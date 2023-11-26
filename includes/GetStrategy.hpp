// FileName: GetStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class GetStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	bool		_is_autoindex;
	std::string	_index_file;
public:
	// Constructors & Destructor
	GetStrategy(ResponseBuildState* state, bool is_autoindex, std::string index_file);
	virtual ~GetStrategy();
	// Methods
	void	buildResponse();
};
