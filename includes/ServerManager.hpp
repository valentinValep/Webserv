/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/10 13:21:43 by chmadran         ###   ########.fr       */
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
# include <sys/poll.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <fstream>
# include <iostream>
# include <algorithm>
# include <fcntl.h>
# include <iomanip>
# include <ctime>


struct SocketInfo {
	int socket;
	time_t lastActivity;
};

class ServerManager {
private:
	int server_fd;
	ClientRequest request;
	struct sockaddr_in address;
	std::vector<Server> servers;
	std::vector<struct pollfd> fds;
	std::vector<SocketInfo> activeSockets; // does not contain server_fd

	void	setupNetwork();
	void	printActiveSockets();
	void	closeInactiveSockets();
	void	updateSocketActivity(int socket);
	void	handleClientRequest(int clientSocket);
	void	parseConfigFile(std::string config_file);
	int		acceptNewConnexion(int server_fd, int &nfds);

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
