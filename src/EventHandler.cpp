#include "EventHandler.hpp"
#include <iostream>
#include <unistd.h>

EventHandler::EventHandler(int socket_fd): _socket_fd(socket_fd), _lastActivity(time(NULL))
{}

EventHandler::~EventHandler()
{
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
	if (time(NULL) - this->_lastActivity > this->getTimeout())
		this->timeout();
}

void EventHandler::updateLastActivity()
{
	this->_lastActivity = time(NULL);
}
