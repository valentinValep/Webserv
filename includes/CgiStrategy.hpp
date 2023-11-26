// FileName: CgiStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class CgiStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	std::string	_extension;
	std::string	_interpreter;
public:
	// Constructors & Destructor
	CgiStrategy(ResponseBuildState* state, std::string extension, std::string interpreter);
	virtual ~CgiStrategy();
	// Methods
	void	buildResponse();
};
