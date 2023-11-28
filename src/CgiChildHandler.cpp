#include "CgiChildHandler.hpp"
#include "ProcessHandler.hpp"
#include "ServerManager.hpp"
#include "ResponseBuildState.hpp"
#include "CgiStrategy.hpp"
#include <typeinfo>

CgiChildHandler::CgiChildHandler(int child_fd, int parent_fd, EventHandler &handler): EventHandler(child_fd), _parent_fd(parent_fd), _handler(handler)
{}

CgiChildHandler::~CgiChildHandler()
{}

void CgiChildHandler::handle()
{
	std::cout << "CgiChildHandler::handle()" << std::endl;
	ServerManager::getInstance()->talkToClient(_parent_fd, _handler);
}

void	CgiChildHandler::timeout()
{
	try {
		std::cout << "CgiChildHandler::timeout()" << std::endl;
		ProcessHandler &parent_handler = dynamic_cast<ProcessHandler &>(this->_handler);
		ResponseBuildState &state = dynamic_cast<ResponseBuildState &>(*parent_handler.getState());
		CgiStrategy &strategy = dynamic_cast<CgiStrategy &>(*state.getStrategy());

		strategy.setTimeout();
		ServerManager::getInstance()->talkToClient(_parent_fd, this->_handler);
	}
	catch (std::bad_cast &e) {
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: " << e.what() << std::endl;
	}
}

time_t CgiChildHandler::getTimeout() const
{
	return CgiChildHandler::_timeout;
}
