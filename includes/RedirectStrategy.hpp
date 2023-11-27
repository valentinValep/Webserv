// FileName: RedirectStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
#include "ResponseBuilder.hpp"
#include <iostream>

class RedirectStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	int			_code;
	std::string	_destination;
public:
	// Constructors & Destructor
	RedirectStrategy(ResponseBuildState* state, int code, std::string destination);
	virtual ~RedirectStrategy();
	// Methods
	void	buildResponse();
};
