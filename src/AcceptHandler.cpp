#include "AcceptHandler.hpp"
#include "ServerManager.hpp"

time_t	AcceptHandler::_timeout = std::numeric_limits<time_t>::max();

AcceptHandler::AcceptHandler(int socket_fd, int port): EventHandler(socket_fd), _port(port)
{}

AcceptHandler::~AcceptHandler()
{}

void AcceptHandler::handle()
{
	int					client_fd;
	struct sockaddr_in	client_address;

	errno = 0;
	client_fd = accept(this->getSocketFd(), (struct sockaddr*)&client_address, (socklen_t []){sizeof(client_address)});
	if (client_fd == -1)
		return perror(SCSTR(__FILE__ << ":" << __LINE__ << " accept() failed"));

	std::cout << "✅ New connection on client_fd: " << client_fd << std::endl;
	ServerManager::getInstance()->addClient(client_fd, _port);
}

void AcceptHandler::timeout()
{
	this->updateLastActivity();
}

time_t AcceptHandler::getTimeout() const
{
	return AcceptHandler::_timeout;
}
