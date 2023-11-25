#include "ProcessHandler.hpp"
#include "ServerManager.hpp"
#include <iostream>

ProcessHandler::ProcessHandler(int socket_fd): EventHandler(socket_fd)
{}

ProcessHandler::~ProcessHandler()
{}

void ProcessHandler::handle()
{
	std::cout << "ProcessHandler::handle()" << std::endl;
}
