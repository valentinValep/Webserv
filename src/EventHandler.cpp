#include "EventHandler.hpp"
#include <iostream>
#include <unistd.h>

EventHandler::EventHandler(int socket_fd): _socket_fd(socket_fd), lastActivity(time(NULL))
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

void EventHandler::handle()
{
	this->updateLastActivity();
}

void EventHandler::checkTimeout()
{
	if (time(NULL) - this->lastActivity > this->getTimeout())
		this->timeout();
}

void EventHandler::updateLastActivity()
{
	this->lastActivity = time(NULL);
}
