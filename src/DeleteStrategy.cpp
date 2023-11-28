#include "DeleteStrategy.hpp"

DeleteStrategy::DeleteStrategy(ResponseBuildState *state): ResponseBuildingStrategy(state)
{}

DeleteStrategy::~DeleteStrategy()
{}

static std::string		trimTrailingSlashes(std::string path)
{
	size_t	index;

	if (!(path.empty()) && ((index = path.find_last_not_of("/")) != path.npos))
			path.erase(index + 1);
	return (path);
}

void DeleteStrategy::buildResponse()
{
	std::string const	locationPath = this->getState()->getRoot() + trimTrailingSlashes(this->getState()->getPath());

	if (access(locationPath.c_str(), F_OK))
	{
		this->setError(404);
		return ;
	}
	if (access(locationPath.c_str(), W_OK))
	{
		this->setError(403);
		return ;
	}

	struct stat		statbuf;

	if (stat(locationPath.c_str(), &statbuf))
	{
		this->setError(500);
		return ;
	}
	if (!S_ISREG(statbuf.st_mode))
	{
		this->setError(403);
		return ;
	}

	if (std::remove(locationPath.c_str()))
	{
		this->setError(500);
		return ;
	}

	ResponseBuilder		builder;

	builder.setCode(204);
	this->setResponse(builder.build());
	this->setAsFinished();
}
