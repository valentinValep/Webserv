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
#include "utils.hpp"

ResponseBuildState::ResponseBuildState(): ProcessState(), _strategy(NULL)
{}

bool requestIsUpload(const ClientRequest &request)
{
	std::string		content_type;

	content_type = request.getHeader("Content-Type");
	return (content_type.find("multipart/form-data") != std::string::npos);
}

static bool	hasExtension(std::string path, std::string extension)
{
	size_t	extension_pos;
	size_t	question_mark_pos;

	question_mark_pos = path.find("?");
	extension_pos = path.rfind(extension);
	if (extension_pos == std::string::npos)
		return (false);
	if (question_mark_pos == std::string::npos)
		return extension_pos + extension.length() == path.length();
	return extension_pos + extension.length() == question_mark_pos;
}

bool	is_method_allowed(const Route *route, const ClientRequest &request)
{
	int		methods_allowed;

	if (route && route->hasMethods())
		methods_allowed = route->getMethods();
	else
		methods_allowed = request.getServer()->getMethods();

	return (methods_allowed & request.getMethod());
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

	route = request.getServer()->getRoute(trimTrailingSlashes(request.getPath()));

	if (!is_method_allowed(route, request))
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
		if (route->hasCgi() && hasExtension(request.getPath(), route->getCgiExtension()))
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
	std::cout << "\t🗑️ Delete ResponseBuildState" << std::endl;
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

ResponseBuildingStrategy *ResponseBuildState::getStrategy()
{
	return this->_strategy;
}

void ResponseBuildState::process()
{
	std::cout << "\t🔨 Build Event" << std::endl;
	if (!this->_strategy)
	{
		std::cout << __FILE__ << ":" << __LINE__ << ": " << "CRITIC: Strategy is NULL" << std::endl;
		return;
	}
	if (this->_strategy->getError())
		this->_strategy = new ErrorStrategy(this, this->_strategy->getError(), this->_error_pages);

	this->_strategy->buildResponse();

	if (this->_strategy->getError())
	{
		int error_code = this->_strategy->getError();
		delete this->_strategy;
		this->_strategy = new ErrorStrategy(this, error_code, this->_error_pages);
		return;
	}
	if (this->_strategy->isFinished())
	{
		this->getHandler()->setState(new ResponseSendState(this->getHandler(), this->getSocketFd(), this->_strategy->getResponse()));
		delete this;
	}
}
