/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:41:09 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/02 15:02:59 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv <optional: config_file>" << std::endl;
		return 1;
	}
	ServerManager webServe(argc == 2 ? argv[1] : "config/default.conf");
	webServe.start();
	return 0;
}
