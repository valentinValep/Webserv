#include "CgiChildHandler.hpp"
#include "ServerManager.hpp"

CgiChildHandler::CgiChildHandler(int child_fd, int parent_fd, EventHandler &handler): EventHandler(child_fd), _parent_fd(parent_fd), _handler(handler)
{}

CgiChildHandler::~CgiChildHandler()
{}

void CgiChildHandler::handle()
{
	std::cout << "CgiChildHandler::handle()" << std::endl;
	ServerManager::getInstance()->talkToClient(_parent_fd, _handler);
}
