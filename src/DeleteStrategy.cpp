#include "DeleteStrategy.hpp"
#include "utils.hpp"

DeleteStrategy::DeleteStrategy(ResponseBuildState *state): ResponseBuildingStrategy(state)
{}

DeleteStrategy::~DeleteStrategy()
{}

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
