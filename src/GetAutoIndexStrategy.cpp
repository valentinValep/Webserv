#include "GetAutoIndexStrategy.hpp"

GetAutoIndexStrategy::GetAutoIndexStrategy(ResponseBuildState *state): ResponseBuildingStrategy(state)
{
	if (!(this->_dirStream = opendir((state->getRoot() + "/" + state->getPath()).c_str())))
	{
		this->setError(500);
		return ;
	}
	this->_autoIndex << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"UTF-8\">\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>"
		<< state->getPath() << "</title>\n\t<link rel=\"stylesheet\" type=\"text/css\" href=\"/style/style.css\">\n\t<link rel=\"stylesheet\" type=\"text/css\" href=\"/style/autoindex.css\">\n\t"
		<< "<link rel=\"icon\" href=\"/favicon.gif\" type=\"image/gif\">\n</head>\n<body>\n\t<h1>Index of " << state->getPath() << "</h1>\n\t<hr>\n";
}

GetAutoIndexStrategy::~GetAutoIndexStrategy()
{
	if (this->_dirStream)
		closedir(this->_dirStream);
}

void GetAutoIndexStrategy::buildResponse()
{
	if (this->getError())	//	check in caller before calling buildResponse() ?
		return ;

	struct dirent	*dirContent = NULL;
	struct stat		fileStat;

	for (int i = 0; i < DIRCHUNKSIZE; i++)
	{
		errno = 0;
		if (!(dirContent = readdir(this->_dirStream)) && errno)
		{
			this->setError(500);
			return ;
		}
		if (!dirContent)
			break ;

		std::string		fileName(dirContent->d_name);
		std::string		locationPath = getState()->getRoot() + getState()->getPath();

		if (stat((locationPath + "/" + fileName).c_str(), &fileStat))
		{
			this->setError(500);
			closedir(this->_dirStream);
			return ;
		}
		this->_autoIndex << "  <a class=\"space" << (S_ISDIR(fileStat.st_mode) ? " dir" : (S_ISREG(fileStat.st_mode) ? " file" : ""))
			<< "\" href=\"" << (locationPath.substr((getState()->getRoot().size())) + "/" + fileName) << "\">" << fileName << "</a><br>\n";
	}
	if (!dirContent)
	{
		this->_autoIndex << "</body>\n</html>\n";

		ResponseBuilder		builder;

		builder.setCode(200);
		builder.addHeader("Content-Type", "text/html");
		builder.setBody(this->_autoIndex.str());
		this->setResponse(builder.build());
		this->setAsFinished();
	}
}
