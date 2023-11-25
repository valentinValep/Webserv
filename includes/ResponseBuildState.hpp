// FileName: ResponseBuildState.hpp

#pragma once

#include "ProcessState.hpp"
#include "ClientRequest.hpp"

class ResponseBuildState: public ProcessState {
private:
	// Attributes
public:
	// Constructors & Destructor
	ResponseBuildState();
	ResponseBuildState(ProcessHandler* handler, int socket_fd, const ClientRequest &request);
	~ResponseBuildState();
	// Methods
	void	process();
};
