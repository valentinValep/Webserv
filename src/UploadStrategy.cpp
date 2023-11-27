#include "UploadStrategy.hpp"
#include <iostream>

UploadStrategy::UploadStrategy(ResponseBuildState *state, std::string upload_dir): ResponseBuildingStrategy(state), _upload_dir(upload_dir)
{}

UploadStrategy::~UploadStrategy()
{
}

void UploadStrategy::buildResponse()
{
	(void)_upload_dir;
	std::cout << "UploadStrategy::buildResponse()" << std::endl;
	this->setAsFinished();
}