// FileName: ResponseBuildingStrategy.hpp

#pragma once

#include "ProcessHandler.hpp"
#include "ClientRequest.hpp"
#include "ResponseBuildState.hpp"
#include "Response.hpp"
#include <iostream>

class ResponseBuildState;

class ResponseBuildingStrategy {
private:
	// Attributes
	int					_error_code;
	bool				_finished;
	Response			_response;
	ResponseBuildState*	_state;
public:
	// Constructors & Destructor
	ResponseBuildingStrategy(ResponseBuildState* state);
	virtual ~ResponseBuildingStrategy();
	// Getters
	int					getError() const;
	Response const		&getResponse() const;
	ResponseBuildState*	getState() const;

	// Setters
	void				setError(int error_code);
	void				setAsFinished();
	void				setResponse(Response const &response);

	// Methods
	bool				isFinished();
	virtual void		buildResponse() = 0;
};
