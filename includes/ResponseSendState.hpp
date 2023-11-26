// FileName: ResponseSendState.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class ResponseSendState: public ProcessState {
private:
	// Attributes
	std::string	_buffer;
public:
	// Constructors & Destructor
	ResponseSendState(ProcessHandler* process_handler, int socket_fd, std::string buffer);
	virtual ~ResponseSendState();
	// Methods
	void	process();
};
