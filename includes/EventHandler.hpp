// FileName: EventHandler.hpp

#pragma once

#include <iostream>

class EventHandler {
private:
	// Attributes
	int		_socket_fd;
	time_t	_lastActivity;
public:
	// Constructors & Destructor
	EventHandler(int socket_fd);
	virtual ~EventHandler();
	// Getters
	int				getSocketFd() const;

	// Setters

	// Methods
	virtual void	handle();
	virtual void	timeout() = 0;
	void			checkTimeout();
	void			updateLastActivity();
	virtual time_t	getTimeout() const = 0;
};
