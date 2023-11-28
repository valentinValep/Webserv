// FileName: DeleteStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
#include "ResponseBuilder.hpp"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>

class DeleteStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
public:
	// Constructors & Destructor
	DeleteStrategy(ResponseBuildState* state);
	virtual ~DeleteStrategy();
	// Methods
	void	buildResponse();
};
