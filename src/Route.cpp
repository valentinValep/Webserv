/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/02 16:47:17 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include <iostream>
#include <string>

// @TODO remove default values
Route::Route(): autoindex(true), methods(), redirect_type(), redirect(), index(""), root(""), cgi_extension(""), cgi_path(""), upload_path("")
{}

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
