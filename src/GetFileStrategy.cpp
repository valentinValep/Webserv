#include "GetFileStrategy.hpp"
#include "Response.hpp"

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
		Response	response;
		std::string	mime_type = this->_file_reader.getMimeType();

		response.setCode(200);
		if (!mime_type.empty())
			response.addHeader("Content-Type", mime_type);
		response.setBody(this->_file_reader.extractFileContent());
		this->setResponse(response);
		this->setAsFinished();
	}
}
