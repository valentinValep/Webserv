// FileName: ProcessHandler.hpp

#pragma once

#include "EventHandler.hpp"
#include "ProcessState.hpp"

// @TODO add timeout for inactive sockets

class ProcessState;

class ProcessHandler: public EventHandler {
private:
	// Attributes
	int				_port;
	ProcessState*	state;
public:
	// Constructors & Destructor
	ProcessHandler(int socket_fd, int port);
	~ProcessHandler();
	// Getters

	// Setters
	void	setState(ProcessState* state);
	int		getPort() const;

	// Methods
	void	handle();
};
