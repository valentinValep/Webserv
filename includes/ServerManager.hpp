/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 15:35:27 by chmadran         ###   ########.fr       */
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

# define GET 1
# define POST 2
# define DELETE 4
# define BUFFER_SIZE 100000


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
	int		acceptNewConnexion(int server_fd)
		__attribute__((warn_unused_result)); // @TODO
	void	cleanFdsAndActiveSockets();

	int		handleClientRequest(ClientRequest &request);
	int		readClientRequest(ClientRequest &request);
	void	storeHeaderClientRequest(char *buffer, int bytesRead, ClientRequest &request);
	void	storeBodyClientRequest(char *buffer, int bytesRead, ClientRequest &request);

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
