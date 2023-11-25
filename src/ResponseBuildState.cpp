#include "ResponseBuildState.hpp"

ResponseBuildState::ResponseBuildState(): ProcessState()
{}

ResponseBuildState::ResponseBuildState(ProcessHandler *handler, int socket_fd, const ClientRequest &request): ProcessState(handler, socket_fd)
{
	// @TODO use request to chose responseStrategy
	(void)request;
}

ResponseBuildState::~ResponseBuildState()
{}

void ResponseBuildState::process()
{
	std::cout << "ResponseBuildState::process()" << std::endl;
}
