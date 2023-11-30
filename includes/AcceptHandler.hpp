// FileName: AcceptHandler.hpp

#pragma once

#include "EventHandler.hpp"
#include <limits>

class AcceptHandler: public EventHandler {
private:
	static time_t	_timeout;
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
	void	timeout();
	time_t	getTimeout() const;
};
