/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 14:47:38 by vlepille          #+#    #+#             */
/*   Updated: 2023/12/01 16:55:56 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "FileParser.hpp"
#include "utils.hpp"
#include <iostream>

#define NO_EVENT 0
#define TIMEOUT 120
#define CR std::cout << std::endl;

std::vector<Server>	parseConfigFile(std::string config_file);

/************************************************************
 *					SINGLETON STUFF							*
 ************************************************************/

ServerManager *ServerManager::_instance = NULL;

ServerManager::ServerManager(std::string configFilePath): servers(parseConfigFile(configFilePath)), reactor(this->servers)
{}

ServerManager *ServerManager::getInstance(std::string config_file)
{
	if (!ServerManager::_instance)
		ServerManager::_instance = new ServerManager(config_file);
	return (ServerManager::_instance);
}

ServerManager::~ServerManager()
{}

/************************************************************
 *						INIT								*
 ************************************************************/

void	ServerManager::deleteInstance(){
	delete _instance;
	_instance = NULL;
};

void	configParser(fp::FileParser &parser)
{
	// Requirement
	parser.require("/server");
	parser.require("/server/listen");
	parser.require("/server/root");
	parser.require("/server/index");
	parser.require("/server/allow_methods");

	// White list
	parser.whitelist("/server/server_name");
	parser.whitelist("/server/client_max_body_size");
	parser.whitelist("/server/autoindex");
	parser.whitelist("/server/error_page");

	parser.whitelist("/server/location/allow_methods");
	parser.whitelist("/server/location/root");
	parser.whitelist("/server/location/index");
	parser.whitelist("/server/location/autoindex");
	parser.whitelist("/server/location/extension");
	parser.whitelist("/server/location/cgi_path");
	parser.whitelist("/server/location/upload_path");
	parser.whitelist("/server/location/return");

	parser.banVariableValue();
	parser.forceModuleName();
}

std::vector<Server>	parseConfigFile(std::string config_file)
try {
	fp::FileParser		parser(config_file);
	fp::Module			*mod;
	std::vector<Server>	servers;

	configParser(parser);
	mod = parser.parse();

	for (std::vector<fp::Object *>::const_iterator it = mod->getObjects().begin(); it != mod->getObjects().end(); it++)
	{
		servers.push_back(Server(*dynamic_cast<fp::Module *>(*it)));
	}
	delete mod;
	return (servers);
}
catch (const fp::FileParser::FileParserException &e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}
catch(const fp::FileParser::FileParserSyntaxException& e)
{
	std::cerr << e.what() << '\n';
	throw ServerManager::ParsingException();
}

/************************************************************
 *						RUN									*
 ************************************************************/

int	ServerManager::addClient(int socket_fd, int port)
{
	return this->reactor.addClient(socket_fd, port);
}

int ServerManager::addCgiChild(int child_fd, int parent_fd, EventHandler &parent_handler)
{
	return this->reactor.addCgiChild(child_fd, parent_fd, parent_handler);
}

void ServerManager::deleteClient(int socket_fd)
{
	this->reactor.deleteClient(socket_fd);
}

void ServerManager::listenClient(int socket_fd, EventHandler &handler)
{
	this->reactor.listenClient(socket_fd, handler);
}

void ServerManager::talkToClient(int socket_fd, EventHandler &handler)
{
	this->reactor.talkToClient(socket_fd, handler);
}

void ServerManager::ignoreClient(int socket_fd)
{
	this->reactor.ignoreClient(socket_fd);
}

void ServerManager::run()
{
	this->reactor.run();
};

Server &ServerManager::getServer(int port)
{
	for (std::vector<Server>::iterator it = this->servers.begin(); it != this->servers.end(); it++)
	{
		if (it->getPort() == port)
			return *it;
	}
	throw ServerManager::ServerNotFoundException();
}

Server &ServerManager::getServer(int port, std::string const &server_name)
{
	for (std::vector<Server>::iterator it = this->servers.begin(); it != this->servers.end(); it++)
	{
		if (it->getPort() == port && it->hasServerName(server_name))
			return *it;
	}
	throw ServerManager::ServerNotFoundException();
}
