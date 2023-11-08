/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/08 19:26:40 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "ClientRequest.hpp"
# include "ServerResponse.hpp"
# include "Server.hpp"
# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <fstream>
# include <iostream>
# include <algorithm>

class ServerManager {
private:
	int server_fd;
	struct sockaddr_in address;
	std::vector<Server> servers;

	void setupNetwork();
	void handleClientRequest(int clientSocket);
	void parseConfigFile(std::string config_file);
public:
	ServerManager(std::string config_file);
	void start();

	class ParsingException : public std::exception {
		virtual const char* what() const throw() {
			return "Parsing error";
		}
	};
};

#endif
