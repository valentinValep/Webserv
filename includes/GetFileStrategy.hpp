// FileName: GetFileStrategy.hpp

#pragma once

#include "ResponseBuildingStrategy.hpp"
#include "FileReader.hpp"

class GetFileStrategy: public ResponseBuildingStrategy {
private:
	// Attributes
	std::string		_file_path;
	FileReader		_file_reader;
public:
	// Constructors & Destructor
	GetFileStrategy(ResponseBuildState* state, std::string file_path);
	virtual ~GetFileStrategy();
	// Methods
	void	buildResponse();
};