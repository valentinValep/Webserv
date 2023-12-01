// FileName: ResponseSendState.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class ResponseSendState: public ProcessState {
private:
	// Attributes
	Response	_response;
public:
	// Constructors & Destructor
	ResponseSendState(ProcessHandler* process_handler, int socket_fd, Response const &response);
	virtual ~ResponseSendState();
	// Methods
	void	process();
};
