/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:41:09 by chmadran          #+#    #+#             */
/*   Updated: 2023/12/01 16:52:21 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include <signal.h>

volatile sig_atomic_t signal_received = false;

void sigint_handler(int signum)
{
	signal_received = true;
	(void)signum;
}

void sigpipe_handler(int signum)
{
	(void)signum;
}

int main(int argc, char **argv)
try {
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv <optional: config_file>" << std::endl;
		return 1;
	}
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);
	ServerManager::getInstance(argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE);
	ServerManager::getInstance()->run();
	ServerManager::deleteInstance();
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
