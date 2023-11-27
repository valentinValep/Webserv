#include "CgiStrategy.hpp"
#include "ResponseBuilder.hpp"
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
	ResponseBuilder	builder;

	setPath();
	setEnv();
	convertEnv();
	executeScript();
	freeEnvp();
	builder.setCode(200);
	builder.addHeader("Content-Type", "text/html");
	builder.setBody(_response);
	this->setResponse(builder.build());
	this->setAsFinished();
	std::cout << "CgiStrategy::buildResponse()" << std::endl;
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
};


/************************************************************
 *						EXECUTION							*
 ************************************************************/

void		CgiStrategy::executeScript()
{
	pipe(this->_fd);
	this->_pid = fork();

	if (this->_pid == 0)
	{
		cgiChildProcess();
	}
	else
	{
		cgiParentProcess();
	}
};

void	CgiStrategy::cgiChildProcess()
{
	_cgiInterpreter = _interpreter.c_str();
	std::string scriptName = this->getState()->getRoot() + _path;
	_scriptName = scriptName.c_str();

	const char *args[] = {_cgiInterpreter, _scriptName, NULL};

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

void	CgiStrategy::cgiParentProcess()
{
	close(this->_fd[WRITE]);
	waitpid(this->_pid, NULL, 0);
	char c;
	while (read(_fd[READ], &c, 1) > 0)
	{
		_response += c;
	}
	close(this->_fd[READ]);
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


