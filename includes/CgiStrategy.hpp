// FileName: CgiStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

#define GET 1
#define POST 2
#define READ 0
#define WRITE 1
#define ERROR -1

class CgiStrategy: public ResponseBuildingStrategy {
private:
		// Attributes
		// ResponseBuildState					*state;
		std::string							_path;
		std::string							_queryString;
		std::string							_response;
		std::string							_cgiBody;
		std::string							_interpreter;
		std::string							_scriptName;
		int									_method;
		char**								_envp;
		std::map<std::string, std::string>	_env;
		int									_fd[2];
		int									_pid;
public:
	// Constructors & Destructor
	CgiStrategy(ResponseBuildState* state, std::string cgiInterpreter, int method);
	virtual ~CgiStrategy();

	// Methods
	void			setEnv();
	void			executeScript();
	void			cgiChildProcess();
	void			cgiParentProcess();
	void			convertEnv();
	void			setPath();
	void			freeEnvp();
	std::string		getResponse();
	int				getContentLength(std::map<std::string, std::string> headers);
	std::string		getContentType(std::map<std::string, std::string> headers);
	void			buildResponse();
	void			setTimeout();
};
