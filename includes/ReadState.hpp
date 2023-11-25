// FileName: ReadState.hpp

#pragma once

#include "ProcessState.hpp"
#include "ClientRequest.hpp"
#include "ServerManager.hpp"

class ReadState: public ProcessState {
private:
	// Attributes
	ClientRequest	request;
	char			buffer[BUFFER_SIZE];

	int		readClientRequest();
public:
	// Constructors & Destructor
	ReadState();
	ReadState(ProcessHandler* handler, int socket_fd);
	~ReadState();
	// Methods
	void	process();
};
