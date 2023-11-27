/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 14:19:07 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/27 12:24:23 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiRequest.hpp"
#include <iostream>

#define GET 1
#define POST 2
#define READ 0
#define WRITE 1
#define ERROR -1

/************************************************************
 *					CONSTRUCTOR/DESTRUCTOR					*
 ************************************************************/

CgiRequest::CgiRequest(ServerResponse &response) : serverResponse(response){
	setPath();
	setEnv();
	convertEnv();
	executeScript();
	freeEnvp();
};

CgiRequest::~CgiRequest(){
};

/************************************************************
 *						ENV STUFF							*
 ************************************************************/

void		CgiRequest::setEnv()
{
	_env["AUTH_TYPE"] = "";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_NAME"] = "webserv";
	_env["METHOD"] = serverResponse.getMethod();
	_env["PORT"] = serverResponse.getPort();
	_env["SCRIPT_FILENAME"] = serverResponse.getRoot() + _path;
	_env["CONTENT_LENGTH"] = serverResponse.getContentLength();
	_env["CONTENT_TYPE"] = serverResponse.getContentType();
	if (serverResponse.getMethod() == GET) {
		_cgiBody = "";
		_env["QUERY_STRING"] = _queryString;
	}
	else if (serverResponse.getMethod() == POST) {
		_env["QUERY_STRING"] = "";
		_cgiBody = _queryString;
	}

};

void	CgiRequest::convertEnv()
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

void	CgiRequest::freeEnvp(){
	for (int i = 0;  _envp[i]; i++)
		delete[] _envp[i];
	delete[] _envp;
};

void	CgiRequest::setPath(){

	std::string	path = serverResponse.getPath();
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

void		CgiRequest::executeScript()
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

void	CgiRequest::cgiChildProcess()
{
	std::string cgiPath = serverResponse.getCgiPath();
	_cgiInterpreter = cgiPath.c_str();

	std::string scriptName = serverResponse.getRoot() + _path;
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

void	CgiRequest::cgiParentProcess()
{
	close(this->_fd[WRITE]);
	waitpid(this->_child_pid, NULL, 0); // @TODO use this->_pid instead of this->_child_pid because it's not initialized
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

std::string CgiRequest::getResponse()
{
	return (this->_response);
};

void	CgiRequest::printResponse()
{
	std::cout << this->_response << std::endl;
};