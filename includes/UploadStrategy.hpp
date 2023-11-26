// FileName: UploadStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"

class UploadStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	std::string	_upload_dir;
public:
	// Constructors & Destructor
	UploadStrategy(ResponseBuildState* state, std::string upload_dir);
	virtual ~UploadStrategy();
	// Methods
	void	buildResponse();
};