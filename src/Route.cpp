/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/30 15:45:50 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include "ServerManager.hpp"
#include <iostream>
#include <string>

/************************************************************
 *					CONSTRUCTORS/DESTRUCTOR					*
 ***********************************************************/

Route::Route(): autoindex(true), methods(), redirect_code(), redirect_dest(), index(""), root(""), cgi_extension(""), cgi_path(""), upload_path(""), _autoindex_set(false), _methods_set(false), _redirect_set(false), _index_set(false), _root_set(false), _cgi_set(false), _upload_set(false)
{}

Route::Route(fp::Module &mod): autoindex(true), methods(), redirect_code(), redirect_dest(), index(""), root(""), cgi_extension(""), cgi_path(""), upload_path(""), _autoindex_set(false), _methods_set(false), _redirect_set(false), _index_set(false), _root_set(false), _cgi_set(false), _upload_set(false)
{
	this->parseMethods(mod);
	this->parseRoot(mod);
	this->parseIndex(mod);
	this->parseAutoindex(mod);
	this->parseCgiExtension(mod);
	this->parseCgiPath(mod);
	this->parseUploadPath(mod);
	this->parseRedirect(mod);
}

Route::~Route()
{}

/************************************************************
 *					GETTERS/HAS								*
 ***********************************************************/

bool Route::getAutoindex() const
{
	return this->autoindex;
}

int Route::getMethods() const
{
	return this->methods;
}

int Route::getRedirectCode() const
{
	return this->redirect_code;
}

std::string Route::getRedirectDest() const
{
	return this->redirect_dest;
}

std::string Route::getIndex() const
{
	return this->index;
}

std::string Route::getRoot() const
{
	return this->root;
}

std::string Route::getCgiExtension() const
{
	return this->cgi_extension;
}

std::string Route::getCgiPath() const
{
	return this->cgi_path;
}

std::string Route::getUploadPath() const
{
	return this->upload_path;
}

bool Route::hasAutoindex() const
{
	return this->_autoindex_set;
}

bool Route::hasMethods() const
{
	return this->_methods_set;
}

bool Route::hasRedirect() const
{
	return this->_redirect_set;
}

bool Route::hasIndex() const
{
	return this->_index_set;
}

bool Route::hasRoot() const
{
	return this->_root_set;
}

bool Route::hasCgi() const
{
	return this->_cgi_set;
}

bool Route::hasUpload() const
{
	return this->_upload_set;
}

/************************************************************
 *							PARSING							*
 ************************************************************/

void Route::parseMethods(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("allow_methods");
	if (!var)
		return ;
	if (var->getAttributes().size() == 0)
	{
		std::cerr << "\033[91mError\033[0m: allow_methods values need to be GET, POST and/or DELETE" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_methods_set = true;
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

void Route::parseRoot(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("root");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: root need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_root_set = true;
	this->root = var->getAttributes()[0];
}

void Route::parseIndex(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("index");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: index need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_index_set = true;
	this->index = var->getAttributes()[0];
}

void Route::parseAutoindex(fp::Module &mod)
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
	this->_autoindex_set = true;
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

void Route::parseCgiExtension(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("extension");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: extension need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_cgi_set = true;
	this->cgi_extension = var->getAttributes()[0];
}

void Route::parseCgiPath(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("cgi_path");
	if (!var)
		return ;
	if (!this->hasCgi())
	{
		std::cerr << "\033[91mError\033[0m: cgi_extension and cgi_path need to be both set" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: cgi_path need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->cgi_path = var->getAttributes()[0];
}

void Route::parseUploadPath(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("upload_path");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "\033[91mError\033[0m: upload_path need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_upload_set = true;
	this->upload_path = var->getAttributes()[0];
}

void Route::parseRedirect(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("return");
	if (!var)
		return ;
	if (var->getAttributes().size() != 2)
	{
		std::cerr << "\033[91mError\033[0m: return need two values" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes()[0] != "301" && var->getAttributes()[0] != "302")
	{
		std::cerr << "\033[91mError\033[0m: return first value need to be 301 or 302" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_redirect_set = true;
	this->redirect_code = var->getAttributes()[0] == "301" ? 301 : 302;
	this->redirect_dest = var->getAttributes()[1];
}
