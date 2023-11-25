// FileName: AcceptHandler.hpp

#pragma once

#include "EventHandler.hpp"

class AcceptHandler: public EventHandler {
private:
	// Attributes
public:
	// Constructors & Destructor
	AcceptHandler(int socket_fd);
	~AcceptHandler();
	// Getters

	// Setters

	// Methods
	void	handle();
};
