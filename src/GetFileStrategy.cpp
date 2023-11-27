#include "GetFileStrategy.hpp"
#include "ResponseBuilder.hpp"

GetFileStrategy::GetFileStrategy(ResponseBuildState *state, std::string file_path): ResponseBuildingStrategy(state), _file_path(file_path)
{}

GetFileStrategy::~GetFileStrategy() {}

void GetFileStrategy::buildResponse()
{
	if (!this->_file_reader.isInitialized())
		this->_file_reader = FileReader(this->_file_path);
	if (!this->_file_reader.isOpen())
		this->setError(404); // 404 ?
	this->_file_reader.readChunk();
	if (this->_file_reader.isTotallyRead())
	{
		ResponseBuilder	builder;

		builder.setCode(200);
		builder.addHeader("Content-Type", this->_file_reader.getMimeType());
		builder.setBody(this->_file_reader.extractFileContent());
		this->setResponse(builder.build());
		this->setAsFinished();
	}
}
