/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/08 19:29:31 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "utils.hpp"

// @TODO remove default values
Server::Server(): port(8080), methods(GET | POST | DELETE), max_body_size(1000000), root("./"), index("index.html")
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

void Server::parsePort(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("port");
	if (!var || var->getAttributes().size() != 1)
	{
		std::cerr << "Error: port need one value" << std::endl;
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

Server::Server(fp::Module &mod)
{
	this->parsePort(mod);
	this->parseMaxBodySize(mod);
	this->parseErrorPages(mod);
	this->parseRoot(mod);
	this->parseIndex(mod);
	this->parseMethods(mod);
	this->parseServerNames(mod);
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
