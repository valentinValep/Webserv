// FileName: ResponseBuildingStrategy.hpp

#pragma once

#include "ProcessHandler.hpp"
#include "ClientRequest.hpp"
#include "ResponseBuildState.hpp"
#include <iostream>

class ResponseBuildState;

class ResponseBuildingStrategy {
private:
	// Attributes
	int					_error_code;
	bool				_finished;
	std::string			_response; // @TODO opti: think about a better way to store the response ?
	ResponseBuildState*	_state;
public:
	// Constructors & Destructor
	ResponseBuildingStrategy(ResponseBuildState* state);
	virtual ~ResponseBuildingStrategy();
	// Getters
	int					getError();
	std::string			getResponse();
	ResponseBuildState*	getState();

	// Setters
	void				setError(int error_code);
	void				setAsFinished();
	void				setResponse(std::string response);

	// Methods
	bool				isFinished();
	virtual void		buildResponse() = 0;
};
