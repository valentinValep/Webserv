// FileName: DeleteStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

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
