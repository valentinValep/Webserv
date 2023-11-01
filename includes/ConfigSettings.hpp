/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigSettings.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 16:44:46 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 11:16:56 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSETTINGS_HPP
#define CONFIGSETTINGS_HPP

#include <string>
#include <vector>

struct Route {
	std::string path;
	std::string methods;
	std::string directory;
	std::string default_file;
	std::string cgi_extensions;
	bool allow_file_uploads;
	std::string upload_directory;
};

struct ConfigSettings {
	int port;
	std::string host;
	std::vector<std::string> server_names;
	std::string error_404_page;
	std::string error_500_page;
	int max_client_body_size;

	std::vector<Route> routes;
};

#endif
