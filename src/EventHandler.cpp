#include "EventHandler.hpp"

EventHandler::EventHandler(int socket_fd)
{
	this->socket_fd = socket_fd;
}

EventHandler::~EventHandler() {}

int EventHandler::getSocketFd() const
{
	return (this->socket_fd);
}
