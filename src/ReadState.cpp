#include "ReadState.hpp"
#include "ServerManager.hpp"
#include "ResponseBuildState.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

ReadState::ReadState(): ProcessState(), request(), buffer()
{
}

ReadState::ReadState(ProcessHandler *handler, int socket_fd): ProcessState(handler, socket_fd), request(this->getSocketFd(), this->getHandler()->getPort()), buffer()
{}

ReadState::~ReadState()
{}

void ReadState::process()
{
	std::cout << "\t📩 Read Event" << std::endl;
	ssize_t bytesRead = 0;
	bytesRead = readClientRequest();
	if (bytesRead <= 0)
		return;

	this->request << std::string(this->buffer, bytesRead);
	this->request.parse();
	this->request.short_print();
	if (this->request.isFullyReceived()
		|| this->request.getErrorCode() == 400)
	{
		ServerManager::getInstance()->talkToClient(this->getSocketFd(), *this->getHandler());
		this->getHandler()->setState(new ResponseBuildState(this->getHandler(), this->getSocketFd(), this->request)); // @TODO check destroy
	}
}

int	ReadState::readClientRequest() {
	ssize_t bytesRead = recv(this->getSocketFd(), this->buffer, BUFFER_SIZE, 0);

	if (bytesRead <= 0) {
		if (bytesRead < 0)
			perror(SCSTR(__FILE__ << ":" << __LINE__ << ": In read"));
		if (bytesRead == 0)
			std::cout << "❌ Connection closed by client [" <<  this->getSocketFd() << "]" << std::endl;
		ServerManager::getInstance()->deleteClient(this->getSocketFd());
		return (bytesRead);
	}
	// std::cout << "\n\n" << "===============   "  << bytesRead << " BYTES  RECEIVED   ===============\n";
	// for (int i = 0; i < bytesRead; i++)
	// 	std::cout << this->buffer[i];
	return (bytesRead);
}
