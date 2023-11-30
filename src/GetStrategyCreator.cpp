#include "GetStrategyCreator.hpp"
#include "ErrorStrategy.hpp"
#include "GetFileStrategy.hpp"
#include "GetAutoIndexStrategy.hpp"
#include "utils.hpp"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

ResponseBuildingStrategy *GetStrategyCreator::createGetStrategy(ResponseBuildState *state, bool is_autoindex, std::string index_file)
{
	std::string const	locationPath = state->getRoot() + trimTrailingSlashes(state->getPath());
	struct stat			fileStat;

	std::cout << "\tlocationPath: " << locationPath << std::endl;
	if(access(locationPath.c_str(), F_OK))
		return (new ErrorStrategy(state, 404, state->getErrorPages()));
	if(access(locationPath.c_str(), R_OK))
		return (new ErrorStrategy(state, 403, state->getErrorPages()));
	if (stat(locationPath.c_str(), &fileStat))
		return (new ErrorStrategy(state, 500, state->getErrorPages()));

	if (S_ISREG(fileStat.st_mode))
		return (new GetFileStrategy(state, locationPath));
	if (S_ISDIR(fileStat.st_mode))
	{
		if (index_file.empty())
		{
			if (!is_autoindex)
				return (new ErrorStrategy(state, 404, state->getErrorPages()));
			return (new GetAutoIndexStrategy(state));
		}
		std::string const	indexPath = locationPath + "/" + index_file;

		if (stat(indexPath.c_str(), &fileStat))
			return (new ErrorStrategy(state, 500, state->getErrorPages()));
		if (!S_ISREG(fileStat.st_mode))
			return (new ErrorStrategy(state, 404, state->getErrorPages()));
		if (!access(indexPath.c_str(), F_OK))
		{
			if (!access(indexPath.c_str(), R_OK))
				return (new GetFileStrategy(state, indexPath));
			return (new ErrorStrategy(state, 403, state->getErrorPages()));
		}
	}
	return (new ErrorStrategy(state, 404, state->getErrorPages()));
}
