// FileName: GetAutoIndexStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class GetAutoIndexStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
public:
	// Constructors & Destructor
	GetAutoIndexStrategy(ResponseBuildState* state);
	virtual ~GetAutoIndexStrategy();
	// Methods
	void	buildResponse();
};