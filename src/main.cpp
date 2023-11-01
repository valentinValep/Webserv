/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:41:09 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 12:14:03 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	setupSettings(ConfigSettings &settings)
{
	settings.port = 8080;
	settings.host = "localhost";
	settings.server_names.push_back("webserv.com");
	settings.server_names.push_back("test.com");
	settings.error_404_page = "errors/404.html";
	settings.error_500_page = "errors/500.html";
	settings.max_client_body_size = 10;

	Route route = {
		"/",
		"GET, POST",
		"/Desktop/REPO/Webserv/conf",
		"index.html",
		".php",
		true,
		"/var/www/uploads"
	};
	settings.routes.push_back(route);
}

int main()
{
	ConfigSettings settings;
	setupSettings(settings);

	Server webServe(settings);
	webServe.start();
	return 0;
}
