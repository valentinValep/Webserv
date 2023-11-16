/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 14:19:07 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/16 14:30:02 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiRequest.hpp"

#define READ 0
#define WRITE 1
#define ERROR -1

/************************************************************
 *					CONSTRUCTOR								*
 ************************************************************/

CgiRequest::CgiRequest(ClientRequest &request){
	setEnv(request);
	convertEnv();
	executeScript();
	freeEnvp();
};

CgiRequest::~CgiRequest(){};

/************************************************************
 *					ENV STUFF								*
 ************************************************************/

void		CgiRequest::setEnv(ClientRequest &request)
{
	_env["AUTH_TYPE"] = "";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REDIRECT_STATUS"] = "200";
	_env["METHOD"] = request.method;
	_env["PORT"] = request.server->getPort();
	_env["SERVER_NAME"] = "webserv";
	_env["CONTENT_LENGTH"] = "0";
	_env["CONTENT_TYPE"] = "0";
	_env["PATH_TRANSLATED"] = "";
	_env["SCRIPT_FILENAME"] = "/www/script.cgi";
	_env["QUERY_STRING"] = "";
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
	const char *args[] = {"/usr/bin/python3", "/mnt/nfs/homes/chmadran/Desktop/repo/www/script.py", NULL};
	// char *env[] = {"REQUEST_METHOD=GET", "QUERY_STRING=user=123", NULL};

	close(this->_fd[READ]);
	dup2(this->_fd[WRITE], STDOUT_FILENO);
	if (execve(args[0], const_cast<char *const *>(args), this->_envp))
		exit(ERROR);
};

void	CgiRequest::cgiParentProcess()
{
	close(this->_fd[WRITE]);
	waitpid(this->_child_pid, NULL, 0);
	char c;
	while (read(_fd[READ], &c, 1) > 0)
	{
		_response += c;
	}
	close(this->_fd[READ]);
};

std::string CgiRequest::getResponse()
{
	return (this->_response);
};

void	CgiRequest::printResponse()
{
	std::cout << this->_response << std::endl;
};