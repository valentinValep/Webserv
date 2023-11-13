/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/13 12:37:53 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "utils.hpp"

// @TODO remove default values
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

void Server::parseAutoindex(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("autoindex");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "Error: autoindex need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes()[0] == "on")
		this->autoindex = true;
	else if (var->getAttributes()[0] == "off")
		this->autoindex = false;
	else
	{
		std::cerr << "Error: autoindex value need to be on or off" << std::endl;
		throw ServerManager::ParsingException();
	}
}

void Server::parsePort(fp::Module &mod)
{
	fp::Variable	*var;

	if (mod.getNbObjects("listen") > 1)
		std::cerr << "Warning: listen is defined multiple times, only the first one will be used" << std::endl;
	var = mod.getVariable("listen");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "Error: listen need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (anti_overflow_atoi(var->getAttributes()[0].c_str(), &this->port))
	{
		std::cerr << "Error: port value need to be an integer" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (this->port < 0 || this->port > 65535)
	{
		std::cerr << "Error: port value need to be between 0 and 65535" << std::endl;
		throw ServerManager::ParsingException();
	}
}

void Server::parseMaxBodySize(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("client_max_body_size");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "Error: client_max_body_size need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (anti_overflow_atoi(var->getAttributes()[0].c_str(), &this->max_body_size))
	{
		std::cerr << "Error: client_max_body_size value need to be an integer" << std::endl;
		throw ServerManager::ParsingException();
	}
}

void Server::parseRoot(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("root");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "Error: root need one value" << std::endl;
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
		std::cerr << "Error: index need one value" << std::endl;
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
			std::cerr << "Error: allow_methods value need to be GET, POST or DELETE" << std::endl;
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
		std::cerr << "Error: server_name need at least one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->server_names.clear();
	for (std::vector<std::string>::const_iterator it = var->getAttributes().begin(); it != var->getAttributes().end(); it++)
	{
		this->server_names.push_back(*it);
	}
}

void Server::parseErrorPages(fp::Module &mod)
{
	for (std::vector<fp::Object *>::const_iterator it = mod.getObjects().begin(); it != mod.getObjects().end(); it++)
	{
		// error_page 400 error/400.html
		if ((*it)->getName() == "error_page")
		{
			fp::Variable	*var;
			int				code;
			std::string		path;

			var = dynamic_cast<fp::Variable *>(*it);
			if (!var || var->getAttributes().size() != 2)
			{
				std::cerr << "Error: error_page need two values" << std::endl;
				throw ServerManager::ParsingException();
			}
			if (anti_overflow_atoi(var->getAttributes()[0].c_str(), &code))
			{
				std::cerr << "Error: error_page code value need to be an integer" << std::endl;
				throw ServerManager::ParsingException();
			}
			if (code < 100 || code > 599) // @TODO check exacts known codes ?
			{
				std::cerr << "Error: error_page code value need to be between 100 and 599" << std::endl;
				throw ServerManager::ParsingException();
			}
			path = var->getAttributes()[1];
			this->error_pages[code] = path;
		}
	}
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
				std::cerr << "Error: location need a path" << std::endl;
				throw ServerManager::ParsingException();
			}
			path = mod->getAttributes()[0];
			this->routes[path] = Route(*mod);
		}
	}
}

// @TODO remove some default values (for each necessary)
Server::Server(fp::Module &mod): autoindex(true), port(80), methods(GET | POST | DELETE), max_body_size(1000000), root("./"), index("index.html")
{
	this->parseAutoindex(mod);
	this->parsePort(mod);
	this->parseMaxBodySize(mod);
	this->parseRoot(mod);
	this->parseIndex(mod);
	// @TODO if methos is not defined, check if routes are defined and if all these routes have methods
	this->parseMethods(mod);
	this->parseServerNames(mod);
	this->parseErrorPages(mod);
	this->parseRoutes(mod);
}

Server::~Server()
{}

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

std::map<std::string, Route> Server::getRoutes() const
{
	return this->routes;
}

std::vector<std::string> Server::getServerNames() const
{
	return this->server_names;
}
