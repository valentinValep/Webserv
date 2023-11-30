#include "ProcessState.hpp"
#include <iostream>

ProcessState::ProcessState(): handler(NULL), socket_fd(-1)
{}

ProcessState::ProcessState(ProcessHandler *handler, int socket_fd): handler(handler), socket_fd(socket_fd)
{}

ProcessState::~ProcessState()
{
	std::cout << "\t🗑️  Deleting ProcessState" << std::endl;
}

ProcessHandler *ProcessState::getHandler() const
{
	return this->handler;
}

int ProcessState::getSocketFd() const
{
	return this->socket_fd;
}
