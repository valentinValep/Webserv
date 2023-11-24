/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/23 14:46:34 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "ClientRequest.hpp"
# include "ServerResponse.hpp"
# include "Server.hpp"
# include <string>
# include <vector>
# include <map>
# include <set>
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
# include <fcntl.h>
# include <unistd.h>

# define GET 1
# define POST 2
# define DELETE 4
# define BUFFER_SIZE 100000

# define DEFAULT_CONFIG_FILE "config/default.conf"
# define HTTP_PROTOCOL "HTTP/1.1"

struct SocketInfo {
	ClientRequest request;
	time_t lastActivity;
};

class ServerManager {
private:
	int							nfds;
	// struct sockaddr_in address;
	std::vector<Server> 		servers;
	std::vector<struct pollfd>	fds;
	std::map<int, int>			listeningSockets; // socket_fd -> port
	std::map<int, SocketInfo>	clientSockets; // socket_fd -> clientRequest
	char 						buffer[BUFFER_SIZE];

	int		setupNetwork();
	void	updateFds(size_t len, std::vector<SocketInfo>& clientSockets);
	void	handleEvent(pollfd &pollfd);
	void	updateSocketActivity(int socket);
	void	parseConfigFile(std::string config_file);
	void	printActiveSockets();
	void	acceptNewConnexion(int server_fd);
	void	cleanFdsAndActiveSockets();
	void	detectTimeOut();
	void	handleClientRequest(ClientRequest &request);
	int		readClientRequest(ClientRequest &request);
	void	setInvalidFd(ClientRequest &request);

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
