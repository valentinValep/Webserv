// FileName: ResponseBuildState.hpp

#pragma once

#include "ProcessState.hpp"
#include "ClientRequest.hpp"
#include "ResponseBuildingStrategy.hpp"

class ResponseBuildingStrategy;

class ResponseBuildState: public ProcessState {
private:
	// Attributes
	ResponseBuildingStrategy*			_strategy;
	std::string							_root;
	std::string							_path;
	std::map<std::string, std::string>	_headers; // @TODO opti: new headers in response and copy the pointer here
	std::map<int, std::string> 			_error_pages;
public:
	// Constructors & Destructor
	ResponseBuildState();
	ResponseBuildState(ProcessHandler* handler, int socket_fd, const ClientRequest &request);
	virtual ~ResponseBuildState();
	// Getters
	std::string							getRoot();
	std::string							getPath();
	std::map<std::string, std::string>	getHeaders();
	std::map<int, std::string>			getErrorPages();

	// Methods
	void	process();
};
