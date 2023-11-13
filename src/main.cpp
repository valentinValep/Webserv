/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:41:09 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/13 16:33:28 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

int main(int argc, char **argv)
try {
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv <optional: config_file>" << std::endl;
		return 1;
	}
	ServerManager webServe(argc == 2 ? argv[1] : "config/default.conf");
	return 0;
}
catch (ServerManager::ParsingException &e) {
	std::cerr << "An error occured while parsing the config file\nExiting..." << std::endl;
	return 2;
}
catch (std::exception &e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
catch (...) {
	std::cerr << "Unknown error" << std::endl;
	return 1;
}
