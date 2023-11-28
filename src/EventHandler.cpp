#include "EventHandler.hpp"
#include <iostream>
#include <unistd.h>

EventHandler::EventHandler(int socket_fd): _socket_fd(socket_fd)
{}

EventHandler::~EventHandler()
{
	// @TODO close socket in destructor ?
	close(this->_socket_fd);
}

int EventHandler::getSocketFd() const
{
	return (this->_socket_fd);
}
