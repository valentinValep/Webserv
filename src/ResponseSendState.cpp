#include "ResponseSendState.hpp"
#include "ReadState.hpp"

ResponseSendState::ResponseSendState(ProcessHandler *process_handler, int socket_fd, std::string buffer): ProcessState(process_handler, socket_fd), _buffer(buffer)
{}


ResponseSendState::~ResponseSendState()
{
	std::cout << "\t🗑️ Delete ResponseSendState" << std::endl;
}

void ResponseSendState::process()
{
	std::cout << "\t📤 Send Event" << std::endl;
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
	ServerManager::getInstance()->listenClient(this->getSocketFd(), *this->getHandler());
	//	If error code == 400, close connection, remove client and return
	//if (_error_code == 400)
	//{
	//	ServerManager::getInstance()->deleteClient(this->getSocketFd());
	//}
	this->getHandler()->setState(new ReadState(this->getHandler(), this->getSocketFd()));
	delete this;
}
