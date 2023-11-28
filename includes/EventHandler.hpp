// FileName: EventHandler.hpp

#pragma once

#include <iostream>

class EventHandler {
private:
	// Attributes
	int		_socket_fd;
public:
	// Constructors & Destructor
	EventHandler(int socket_fd);
	virtual ~EventHandler();
	// Getters
	int				getSocketFd() const;

	// Setters

	// Methods
	virtual void	handle() = 0;
};
