/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:41:09 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/22 14:27:23 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
//#include <iostream>
//#include <csignal>

//__attribute__((destructor))
//void destroy()
//{
//	std::cout << "Destructor called" << std::endl;
//}

//void sigint_handler(int signum)
//{
//	(void)signum;
//	std::cout << "SIGINT received, exiting..." << std::endl;
//	exit(0);
//}

int main(int argc, char **argv)
try {
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv <optional: config_file>" << std::endl;
		return 1;
	}
	//signal(SIGINT, sigint_handler);
	ServerManager webServe(argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE);
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
