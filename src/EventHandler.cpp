#include "EventHandler.hpp"
#include <iostream>
#include <unistd.h>

EventHandler::EventHandler(int socket_fd, int port): _socket_fd(socket_fd), _port(port)
{}

EventHandler::~EventHandler()
{
	close(this->_socket_fd);
}

int EventHandler::getSocketFd() const
{
	return (this->_socket_fd);
}

int EventHandler::getPort() const
{
	return (this->_port);
}
