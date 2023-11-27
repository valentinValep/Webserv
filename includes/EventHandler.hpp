// FileName: EventHandler.hpp

#pragma once

#include <iostream>

class EventHandler {
private:
	// Attributes
	int		_socket_fd;
	int		_port;
public:
	// Constructors & Destructor
	EventHandler(int socket_fd, int port);
	virtual ~EventHandler();
	// Getters
	int				getSocketFd() const;
	int				getPort() const;

	// Setters

	// Methods
	virtual void	handle() = 0;
};
