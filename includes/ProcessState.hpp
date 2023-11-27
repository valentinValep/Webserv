// FileName: ProcessState.hpp

#pragma once

#include "ProcessHandler.hpp"

class ProcessHandler;

class ProcessState {
private:
	// Attributes
	ProcessHandler*		handler;
	int					socket_fd;
public:
	// Constructors & Destructor
	ProcessState();
	ProcessState(ProcessHandler* handler, int socket_fd);
	virtual ~ProcessState();
	// Getters
	ProcessHandler*		getHandler() const;
	int					getSocketFd() const;
	// Methods
	virtual void	process() = 0;
};
