// FileName: CgiChildHandler.hpp

#pragma once

#include "EventHandler.hpp"

class CgiChildHandler: public EventHandler {
private:
	// Attributes
	int				_parent_fd;
	EventHandler	&_handler;
public:
	// Constructors & Destructor
	CgiChildHandler(int child_fd, int parent_fd, EventHandler &handler);
	~CgiChildHandler();
	// Getters

	// Setters

	// Methods
	void	handle();
};