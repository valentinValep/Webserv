// FileName: ProcessHandler.hpp

#pragma once

#include "EventHandler.hpp"

// @TODO add timeout for inactive sockets

class ProcessHandler: public EventHandler {
private:
	// Attributes
public:
	// Constructors & Destructor
	ProcessHandler(int socket_fd);
	~ProcessHandler();
	// Getters

	// Setters

	// Methods
	void	handle();
};
