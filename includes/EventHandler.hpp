// FileName: EventHandler.hpp

#pragma once

#include <iostream>

class EventHandler {
private:
	// Attributes
	int		socket_fd;
public:
	// Constructors & Destructor
	EventHandler(int socket_fd);
	// @TODO close socket in destructor ?
	virtual ~EventHandler();
	// Getters
	int				getSocketFd() const;

	// Setters

	// Methods
	virtual void	handle() = 0;
};
