/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/20 12:31:10 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include "ServerManager.hpp"
#include <iostream>
#include <string>

Route::Route(): autoindex(true), methods(), redirect_type(), redirect(), index(""), root(""), cgi_extension(""), cgi_path(""), upload_path(""), _autoindex_set(false), _methods_set(false), _redirect_set(false), _index_set(false), _root_set(false), _cgi_set(false), _upload_set(false)
{}

Route::Route(fp::Module &mod): autoindex(true), methods(), redirect_type(), redirect(), index(""), root(""), cgi_extension(""), cgi_path(""), upload_path(""), _autoindex_set(false), _methods_set(false), _redirect_set(false), _index_set(false), _root_set(false), _cgi_set(false), _upload_set(false)
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

bool Route::getAutoindex() const
{
	return this->autoindex;
}

int Route::getMethods() const
{
	return this->methods;
}

int Route::getRedirectType() const
{
	return this->redirect_type;
}

std::string Route::getRedirect() const
{
	return this->redirect;
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

void Route::parseMethods(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("allow_methods");
	if (!var)
		return ;
	if (var->getAttributes().size() == 0)
	{
		std::cerr << "Error: allow_methods values need to be GET, POST and/or DELETE" << std::endl;
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
			std::cerr << "Error: allow_methods value need to be GET, POST or DELETE" << std::endl;
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
		std::cerr << "Error: root need one value" << std::endl;
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
		std::cerr << "Error: index need one value" << std::endl;
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
		std::cerr << "Error: autoindex need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_autoindex_set = true;
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

void Route::parseCgiExtension(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("extension");
	if (!var)
		return ;
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "Error: extension need one value" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_cgi_set = true;
	this->cgi_extension = var->getAttributes()[0];
	for (int i = 0; i < ACCEPTED_CGI_TABSIZE; i++)
	{
		if (this->cgi_extension == ACCEPTED_CGI[i])
			return ;
	}
	std::cerr << "Error: extension value need to be one of the following: ";
	for (int i = 0; i < ACCEPTED_CGI_TABSIZE; i++)
	{
		std::cerr << ACCEPTED_CGI[i];
		if (i < ACCEPTED_CGI_TABSIZE - 1)
			std::cerr << ", ";
	}
	std::cerr << std::endl;
	throw ServerManager::ParsingException();
}

void Route::parseCgiPath(fp::Module &mod)
{
	fp::Variable	*var;

	var = mod.getVariable("cgi_path");
	if (!var)
		return ;
	if (!this->hasCgi())
	{
		std::cerr << "Error: cgi_extension and cgi_path need to be both set" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes().size() != 1)
	{
		std::cerr << "Error: cgi_path need one value" << std::endl;
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
		std::cerr << "Error: upload_path need one value" << std::endl;
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
		std::cerr << "Error: return need two values" << std::endl;
		throw ServerManager::ParsingException();
	}
	if (var->getAttributes()[0] != "301" && var->getAttributes()[0] != "302")
	{
		std::cerr << "Error: return first value need to be 301 or 302" << std::endl;
		throw ServerManager::ParsingException();
	}
	this->_redirect_set = true;
	this->redirect_type = var->getAttributes()[0] == "301" ? 301 : 302;
	this->redirect = var->getAttributes()[1];
}
