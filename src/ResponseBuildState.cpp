#include "ResponseBuildState.hpp"
#include "ClientRequest.hpp"
#include "ServerManager.hpp"
#include "Route.hpp"
#include "ErrorStrategy.hpp"
#include "RedirectStrategy.hpp"
#include "CgiStrategy.hpp"
#include "UploadStrategy.hpp"
#include "DeleteStrategy.hpp"
#include "GetStrategyCreator.hpp"
#include "ResponseSendState.hpp"

ResponseBuildState::ResponseBuildState(): ProcessState(), _strategy(NULL)
{}

bool requestIsUpload(const ClientRequest &request)
{
	std::string		content_type;

	content_type = request.getHeader("Content-Type");
	return (content_type.find("multipart/form-data") != std::string::npos);
}

ResponseBuildState::ResponseBuildState(ProcessHandler *handler, int socket_fd, const ClientRequest &request): ProcessState(handler, socket_fd), _strategy(NULL), _root(""), _path("")
{
	const Route	*route;

	if (request.getServer() == NULL)
	{
		std::cout << __FILE__ << ":" << __LINE__ << ": " << " CRITIC !! Server is NULL" << std::endl;
		this->_strategy = new ErrorStrategy(this, 500, std::map<int, std::string>());
		return;
	}

	this->_error_pages = request.getServer()->getErrorPages();
	this->_headers = request.getHeaders();
	this->_path = request.getPath();
	this->_root = request.getServer()->getRoot();
	
	if (request.getErrorCode())
	{
		this->_strategy = new ErrorStrategy(this, request.getErrorCode(), this->_error_pages);
		return;
	}

	route = request.getServer()->getRoute(request.getPath());

	if ((route && route->hasMethods() && !(route->getMethods() & request.getMethod()))
		|| !(request.getServer()->getMethods() & request.getMethod()))
	{
		this->_strategy = new ErrorStrategy(this, 405, this->_error_pages);
		return;
	}

	if (route)
	{
		if (route->hasRoot())
			this->_root = route->getRoot();
		if (route->hasRedirect())
		{
			this->_strategy = new RedirectStrategy(this, route->getRedirectCode(), route->getRedirectDest());
			return;
		}
		if (route->hasCgi()) // @TODO discuss finding page without cgi extension
		{
			this->_strategy = new CgiStrategy(this, route->getCgiPath(), request.getMethod());
			return;
		}
		if (requestIsUpload(request))
		{
			if (route->hasUpload())
				this->_strategy = new UploadStrategy(this, route->getUploadPath(), request.getBodyBody(), request.getHeaders());
			else
				this->_strategy = new ErrorStrategy(this, 403, this->_error_pages);
			return;
		}
	}
	if (request.getMethod() == DELETE)
	{
		this->_strategy = new DeleteStrategy(this);
		return;
	}
	if (request.getMethod() == GET)
	{
		bool			autoindex;
		std::string		index;

		if (route && route->hasAutoindex())
			autoindex = route->getAutoindex();
		else
			autoindex = request.getServer()->getAutoindex();

		if (route && route->hasIndex())
			index = route->getIndex();
		else if (route && request.getPath() == "/")
			index = request.getServer()->getIndex();
		else if (route)
			index = "";
		else
			index = request.getServer()->getIndex();

		this->_strategy = GetStrategyCreator::createGetStrategy(this, autoindex, index);
		return;
	}
	this->_strategy = new ErrorStrategy(this, 501, this->_error_pages);
}

ResponseBuildState::~ResponseBuildState()
{
	if (this->_strategy)
		delete this->_strategy;
}

std::string ResponseBuildState::getRoot()
{
	return this->_root;
}

std::string ResponseBuildState::getPath()
{
	return this->_path;
}

std::string ResponseBuildState::getPort(){
	return this->_port;
}

std::map<std::string, std::string> ResponseBuildState::getHeaders()
{
	return this->_headers;
}

std::map<int, std::string> ResponseBuildState::getErrorPages()
{
	return this->_error_pages;
}

void ResponseBuildState::process()
{
	if (!this->_strategy)
	{
		std::cout << __FILE__ << ":" << __LINE__ << ": " << "CRITIC: Strategy is NULL" << std::endl;
		return;
	}
	if (this->_strategy->getError())
		this->_strategy = new ErrorStrategy(this, this->_strategy->getError(), this->_error_pages);

	this->_strategy->buildResponse();

	if (this->_strategy->isFinished())
	{
		this->getHandler()->setState(new ResponseSendState(this->getHandler(), this->getSocketFd(), this->_strategy->getResponse()));
		return;
	}
	if (this->_strategy->getError())
	{
		this->_strategy = new ErrorStrategy(this, this->_strategy->getError(), this->_error_pages);
		return;
	}
}
