#include "ResponseSendState.hpp"
#include "ReadState.hpp"

ResponseSendState::ResponseSendState(ProcessHandler *process_handler, int socket_fd, Response const &response): ProcessState(process_handler, socket_fd), _response(response)
{}


ResponseSendState::~ResponseSendState()
{
	std::cout << "\t🗑️ Delete ResponseSendState" << std::endl;
}

void ResponseSendState::process()
{
	std::cout << "\t📤 Send Event" << std::endl;
	int				ret;
	std::string		response = this->_response.build();

	// @TODO send in chunks
	ret = send(getSocketFd(), response.c_str(), response.length(), 0);
	if (ret == -1)
	{
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: send() failed" << std::endl;
		return ;
	}
	if (ret != (int)response.length())
	{
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: send() doesn't send all the buffer (ret = " << ret << ", buffer.length() = " << response.length() << ")" << std::endl;
		return ;
	}
	ServerManager::getInstance()->listenClient(this->getSocketFd(), *this->getHandler());
	//	If error code == 400, close connection, remove client and return
	if (this->_response.getCode() == 400)
	{
		ServerManager::getInstance()->deleteClient(this->getSocketFd());
		return ;
	}
	this->getHandler()->setState(new ReadState(this->getHandler(), this->getSocketFd()));
	delete this;
}
