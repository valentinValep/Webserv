// FileName: GetAutoIndexStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
#include "Response.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define DIRCHUNKSIZE	10

class GetAutoIndexStrategy: public ResponseBuildingStrategy {
private:
	DIR					*_dirStream;
	std::stringstream	_autoIndex;
public:
	// Constructors & Destructor
	GetAutoIndexStrategy(ResponseBuildState* state);
	virtual ~GetAutoIndexStrategy();
	// Methods
	void	buildResponse();
};
