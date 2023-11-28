#include "CgiStrategy.hpp"
#include "ResponseBuilder.hpp"
#include "ServerManager.hpp"
#include <iostream>

/************************************************************
 *					CONSTRUCTOR/DESTRUCTOR					*
 ************************************************************/

CgiStrategy::CgiStrategy(ResponseBuildState *state, std::string cgiInterpreter, int method): ResponseBuildingStrategy(state), _interpreter(cgiInterpreter), _method(method)
{

}

CgiStrategy::~CgiStrategy()
{
}

void CgiStrategy::buildResponse()
{
	if (this->_scriptName.empty())
	{
		setPath();
		if (this->getError())
			return;
		setEnv();
		convertEnv();
		executeScript();
		freeEnvp();
		close(this->_fd[WRITE]);
		ServerManager::getInstance()->addCgiChild(this->_fd[READ], this->getState()->getSocketFd(), *this->getState()->getHandler());
		ServerManager::getInstance()->ignoreClient(this->getState()->getSocketFd());
	}
	else
	{
		int		status;
		char	buf[4];
		bool	is_finished;
		int		read_ret;

		is_finished = waitpid(this->_pid, &status, WNOHANG);
		while ((read_ret = read(this->_fd[READ], buf, 4)) > 0)
			this->_response += std::string(buf, read_ret);
		if (!is_finished)
		{
			ServerManager::getInstance()->ignoreClient(this->getState()->getSocketFd());
			return;
		}
		std::cout << "CGI STATUS: " << WEXITSTATUS(status) << std::endl;
		if (WEXITSTATUS(status) == ERROR)
		{
			std::cout << "CGI ERROR: " << WEXITSTATUS(status) << std::endl;
			return;
		}
		close(this->_fd[READ]);
		// @TODO delete cgi child handler

		{
			ResponseBuilder	builder;

			if (access(_interpreter.c_str(), F_OK) || access(_interpreter.c_str(), X_OK))
			{
				this->setError(502);
				return;
			}

			if (this->_response.empty())
			{
				this->setError(500); // @TODO check NGINX behavior
				return;
			}
			builder.setCode(200);
			builder.addHeader("Content-Type", "text/html");
			builder.setBody(this->_response);
			this->setResponse(builder.build());
			this->setAsFinished();
		}
	}
}

void CgiStrategy::setTimeout()
{
	errno = 0;
	if (kill(this->_pid, SIGKILL) == -1)
		std::cerr << __FILE__ << ":" << __LINE__ << ": Error: kill(): " << strerror(errno) << std::endl;
}

/************************************************************
 *						ENV STUFF							*
 ************************************************************/

void		CgiStrategy::setEnv()
{
	_env["AUTH_TYPE"] = "";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_NAME"] = "webserv";
	_env["METHOD"] = _method;
	_env["PORT"] = this->getState()->getPort();
	_env["SCRIPT_FILENAME"] = this->getState()->getRoot() + _path;
	_env["CONTENT_LENGTH"] = getContentLength(this->getState()->getHeaders());
	_env["CONTENT_TYPE"] = getContentType(this->getState()->getHeaders());
	if (_method == GET) {
		_cgiBody = "";
		_env["QUERY_STRING"] = _queryString;
	}
	else if (_method == POST) {
		_env["QUERY_STRING"] = "";
		_cgiBody = _queryString;
	}
};

void	CgiStrategy::convertEnv()
{
	char **envp = new char*[_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		envp[i] = new char[tmp.size() + 1];
		strcpy(envp[i], tmp.c_str());
		i++;
	}
 	envp[i] = NULL;
	_envp = envp;
};

void	CgiStrategy::freeEnvp(){
	for (int i = 0;  _envp[i]; i++)
		delete[] _envp[i];
	delete[] _envp;
};

void	CgiStrategy::setPath(){

	std::string	path = "";
	path = this->getState()->getPath();
	_queryString = "";
	size_t queryPos = path.find("?");
	if (queryPos != std::string::npos)
	{
		_path = path.substr(0, queryPos);
		_queryString = path.substr(queryPos + 1);
	}
	else
	{
		_path = path;
	}

	this->_scriptName = this->getState()->getRoot() + _path;

	if (access(_scriptName.c_str(), F_OK))
	{
		this->setError(404);
		return;
	}
	if (access(_scriptName.c_str(), R_OK))
	{
		this->setError(403);
	}
};


/************************************************************
 *						EXECUTION							*
 ************************************************************/

void		CgiStrategy::executeScript()
{
	pipe(this->_fd);
	this->_pid = fork();

	if (this->_pid == 0)
		cgiChildProcess();
};

void	CgiStrategy::cgiChildProcess()
{
	const char *args[] = {_interpreter.c_str(), this->_scriptName.c_str(), NULL};

	close(this->_fd[READ]);
	dup2(this->_fd[WRITE], STDOUT_FILENO);

	if (_cgiBody.length() > 0)
	{
		int _fd2[2];
		pipe(_fd2);
		dup2(_fd2[READ], STDIN_FILENO);
		write(_fd2[WRITE], _cgiBody.c_str(), _cgiBody.length());
		close(_fd2[WRITE]);
	}
	if (execve(args[0], const_cast<char *const *>(args), this->_envp))
		exit(ERROR);
};

/************************************************************
 *						UTILS								*
 ***********************************************************/

int	CgiStrategy::getContentLength(std::map<std::string, std::string> headers) {
	std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
	if (it != headers.end())
		return (atoi(it->second.c_str()));
	return (0);
}

std::string	CgiStrategy::getContentType(std::map<std::string, std::string> headers) {
	std::string contentType = "";
	std::map<std::string, std::string>::iterator it = headers.find("Content-Type");
	if (it != headers.end())
		contentType = it->second;
	return (contentType);
}
