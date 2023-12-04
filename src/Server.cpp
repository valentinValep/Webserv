/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/12/04 14:20:51 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "utils.hpp"
#include <limits>

/************************************************************
 *					CONSTRUCTORS/DESTRUCTOR					*
 ************************************************************/

Server::Server(): autoindex(true), port(8080), methods(GET | POST | DELETE), max_body_size(1000000), root("./"), index("index.html")
{
	error_pages[400] = "error/400.html";
	error_pages[403] = "error/403.html";
	error_pages[404] = "error/404.html";
	error_pages[405] = "error/405.html";
	error_pages[413] = "error/413.html";
	error_pages[500] = "error/500.html";
	error_pages[501] = "error/501.html";
	error_pages[505] = "error/505.html";

	routes["/"] = Route();

	server_names.push_back("localhost");
	server_names.push_back("norminet");
}

Server::Server(fp::Module &mod): autoindex(true), port(80), methods(0), max_body_size(-1), root("./"), index("index.html")
{
	this->parseAutoindex(mod);
	this->parsePort(mod);
	this->parseMaxBodySize(mod);
	this->parseRoot(mod);
	this->parseIndex(mod);
	this->parseMethods(mod);
	this->parseServerNames(mod);
	this->parseErrorPages(mod);
	this->parseRoutes(mod);
}

Server::~Server(){};

/************************************************************
 *							PARSERS							*
 ************************************************************/

void Server::parseAutoindex(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("autoindex");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: autoindex need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes()[0] == "on")
		this->autoindex = true;
	else if (var->getAttributes()[0] == "off")
		this->autoindex = false;
	else
	{
		std::cerr << "\033[91mError\033[0m: autoindex value need to be on or off" << std::endl;
		throw ServerManager::ParsingException();
	}
}

void Server::parsePort(fp::Module &mod)
{
	fp::Variable	*var;

	if (mod.getNbObjects("listen") > 1)
		std::cerr << "\033[93mWarning\033[0m: listen is defined multiple times, only the first one will be used" << std::endl;
	var = mod.getVariable("listen");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: listen need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (anti_overflow_atoi(var->getAttributes()[0].c_str(), &this->port))
	{
		std::cerr << "\033[91mError\033[0m: port value need to be an integer" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (this->port < 0 || this->port > 65535)
	{
		std::cerr << "\033[91mError\033[0m: port value need to be between 0 and 65535" << std::endl;
		throw ServerManager::ParsingException();
	}
}

void Server::parseMaxBodySize(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("client_max_body_size");
	if (!var)
	{
		this->max_body_size = -1;
		return ;
	}
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: client_max_body_size need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	errno = 0;
	this->max_body_size = std::strtol(var->getAttributes()[0].c_str(), NULL, 10);
	if (this->max_body_size < 0)
	{
		std::cerr << "\033[91mError\033[0m: client_max_body_size value need to be a positive integer" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (errno == ERANGE)
	{
		std::cerr << "\033[93mWarning\033[0m: client_max_body_size value is out of range (max value is " << std::numeric_limits<long>::max() << ")" << " ignoring it" << std::endl;
		this->max_body_size = -1;
	}
}

void Server::parseRoot(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("root");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: root need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->root = var->getAttributes()[0];
}

void Server::parseIndex(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("index");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: index need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->index = var->getAttributes()[0];
}

void Server::parseMethods(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("allow_methods");
	if (!var || var->getAttributes().size() == 0)
		return ;
	this->methods = 0;
	for (std::vector<std::string>::const_iterator it = var->getAttributes().begin(); it != var->getAttributes().end(); it++)
	{
		if (*it == "GET")
			this->methods |= GET;
		else if (*it == "POST")
			this->methods |= POST;
		else if (*it == "DELETE")
			this->methods |= DELETE;
		else
		{
			std::cerr << "\033[91mError\033[0m: allow_methods value need to be GET, POST or DELETE" << std::endl;
			throw ServerManager::ParsingException();
		}
	}
}

void Server::parseServerNames(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("server_name");
	if (!var)
		return ;
	if (var->getAttributes().size() == 0)
	{
		std::cerr << "\033[91mError\033[0m: server_name need at least one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->server_names.clear();
	for (std::vector<std::string>::const_iterator it = var->getAttributes().begin(); it != var->getAttributes().end(); it++)
	{
		this->server_names.push_back(*it);
	}
}

void	Server::warningErrorPages(void)
{
	for (std::map<int, std::string>::iterator it = this->error_pages.begin(); it != this->error_pages.end(); it++)
	{
		if (access((this->root + "/" + it->second).c_str(), F_OK | R_OK) == -1)
		{
			std::cerr << "\033[93mWarning\033[0m: error_page " << it->first << " file " << this->root + "/" + it->second << " doesn't exist or is not readable" << std::endl;
		}
	}

	static const int basic_codes[] = {400, 403, 404, 405, 413, 500, 501, 505};

	for (int i = 0; i < 8; i++)
	{
		if (this->error_pages.find(basic_codes[i]) == this->error_pages.end())
			std::cerr << "\033[93mWarning\033[0m: error_page " << basic_codes[i] << " is not defined (think about defining it)" << std::endl;
	}
}

void Server::parseErrorPages(fp::Module &mod)
{
	for (std::vector<fp::Object *>::const_iterator it = mod.getObjects().begin(); it != mod.getObjects().end(); it++)
	{
		if ((*it)->getName() == "error_page")
		{
			fp::Variable	*var;
			int				code;
			std::string		path;

			var = dynamic_cast<fp::Variable *>(*it);
			if (!var || var->getAttributes().size() != 2)
			{
				std::cerr << "\033[91mError\033[0m: error_page need two values" << std::endl;
				throw ServerManager::ParsingException();
			}
			if (anti_overflow_atoi(var->getAttributes()[0].c_str(), &code))
			{
				std::cerr << "\033[91mError\033[0m: error_page code value need to be an integer" << std::endl;
				throw ServerManager::ParsingException();
			}
			if (code < 400 || code > 527)
			{
				std::cerr << "\033[91mError\033[0m: error_page code value need to be between 100 and 599" << std::endl;
				throw ServerManager::ParsingException();
			}
			path = var->getAttributes()[1];
			this->error_pages[code] = path;
		}
	}
	this->warningErrorPages();
}

void Server::parseRoutes(fp::Module &mod)
{
	for (std::vector<fp::Object *>::const_iterator it = mod.getObjects().begin(); it != mod.getObjects().end(); it++)
	{
		// location / {
		if ((*it)->getName() == "location")
		{
			fp::Module	*mod;
			std::string	path;

			mod = dynamic_cast<fp::Module *>(*it);
			if (!mod || mod->getAttributes().size() != 1)
			{
				std::cerr << "\033[91mError\033[0m: location need a path" << std::endl;
				throw ServerManager::ParsingException();
			}
			path = trimTrailingSlashes(mod->getAttributes()[0]);
			if (path[0] != '/')
			{
				std::cerr << "\033[91mError\033[0m: location path need to start with a /" << std::endl;
				std::cerr << "Sugesstion: use location '\033[1m/" << path << "\033[0m' instead of location '\033[1m/" << path << "\033[0m'" << std::endl;
				throw ServerManager::ParsingException();
			}
			this->routes[path] = Route(*mod);
		}
	}
}

/************************************************************
 *							GETTERS							*
 ************************************************************/

bool Server::getAutoindex() const
{
	return this->autoindex;
}

int Server::getPort() const
{
	return this->port;
}

int Server::getMethods() const
{
	return this->methods;
}

long Server::getMaxBodySize() const
{
	return this->max_body_size;
}

std::string Server::getRoot() const
{
	return this->root;
}

std::string Server::getIndex() const
{
	return this->index;
}

std::map<int, std::string> Server::getErrorPages() const
{
	return this->error_pages;
}

const Route	*Server::getRoute(const std::string &name) const
{
	for (int i = name.length(); i >= 0; i--)
	{
		std::map<std::string, Route>::const_iterator it = this->routes.find(name.substr(0, i));
		if (it != this->routes.end())
			return &it->second;
	}
	return NULL;
}

std::vector<std::string> Server::getServerNames() const
{
	return this->server_names;
}

bool Server::hasServerName(const std::string &serverName) const
{
	return (std::find(this->server_names.begin(), this->server_names.end(), serverName) != this->server_names.end());
}
