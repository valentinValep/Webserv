// FileName: AcceptHandler.hpp

#pragma once

#include "EventHandler.hpp"

class AcceptHandler: public EventHandler {
private:
	// Attributes
	int		_port;
public:
	// Constructors & Destructor
	AcceptHandler(int socket_fd, int port);
	~AcceptHandler();
	// Getters

	// Setters

	// Methods
	void	handle();
};
