// FileName: GetFileStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class GetFileStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	std::string		_file_path;
public:
	// Constructors & Destructor
	GetFileStrategy(ResponseBuildState* state, std::string file_path);
	virtual ~GetFileStrategy();
	// Methods
	void	buildResponse();
};