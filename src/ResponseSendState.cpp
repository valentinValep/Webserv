#include "ResponseSendState.hpp"
#include "ReadState.hpp"

ResponseSendState::ResponseSendState(ProcessHandler *process_handler, int socket_fd, std::string buffer): ProcessState(process_handler, socket_fd), _buffer(buffer)
{}


ResponseSendState::~ResponseSendState()
{}

void ResponseSendState::process()
{
	int		ret;

	// @TODO send in chunks
	ret = send(getSocketFd(), _buffer.c_str(), _buffer.length(), 0);
	if (ret == -1)
	{
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: send() failed" << std::endl;
		return ;
	}
	if (ret != (int)_buffer.length())
	{
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: send() doesn't send all the buffer (ret = " << ret << ", buffer.length() = " << _buffer.length() << ")" << std::endl;
		return ;
	}
	this->getHandler()->setState(new ReadState(this->getHandler(), this->getSocketFd()));
	ServerManager::getInstance()->listenClient(this->getSocketFd(), *this->getHandler());
}
