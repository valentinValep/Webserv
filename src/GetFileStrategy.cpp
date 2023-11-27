#include "GetFileStrategy.hpp"
#include "ResponseBuilder.hpp"

GetFileStrategy::GetFileStrategy(ResponseBuildState *state, std::string file_path): ResponseBuildingStrategy(state), _file_path(file_path)
{}

GetFileStrategy::~GetFileStrategy() {}

void GetFileStrategy::buildResponse()
{
	ResponseBuilder	builder;

	builder.setCode(200);
	builder.addHeader("Content-Type", "text/html");
	builder.setBody("<html><body><h1>File</h1></body></html>");
	this->setResponse(builder.build());
	this->setAsFinished();
}
