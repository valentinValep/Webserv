/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/12/01 19:16:10 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "ClientRequest.hpp"
# include "ServerResponse.hpp"
# include "Server.hpp"
# include "ServerReactor.hpp"
# include "EventHandler.hpp"
# include <string>
# include <vector>
# include <map>
# include <set>
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

# define QUEUE_SIZE 100
# define EPOLL_CREATION_SOCKET_COUNT 100
# define MAX_CONNECTION 1020 // always < 1024

#define SCSTR( x ) static_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str().c_str()

#define SSTR( x ) static_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

class ServerManager {
private:
	static ServerManager		*_instance;
	std::vector<Server> 		servers;
	ServerReactor				reactor;

	ServerManager(std::string config_file);

public:
	static ServerManager *getInstance(std::string config_file = DEFAULT_CONFIG_FILE);
	~ServerManager();

	int		addClient(int socket_fd, int port);
	int		addCgiChild(int child_fd, int parent_fd, EventHandler &parent_handler);
	void	deleteClient(int socket_fd);
	static void	deleteInstance();
	void	listenClient(int socket_fd, EventHandler &handler);
	void	talkToClient(int socket_fd, EventHandler &handler);
	void	ignoreClient(int socket_fd);
	void	run();

	Server	&getServer(int port);
	Server	&getServer(int port, std::string const &server_name);

	class ParsingException : public std::exception {
		virtual const char* what() const throw() {
			return "Parsing error";
		}
	};

	class ServerNotFoundException : public std::exception {
		virtual const char* what() const throw() {
			return "No server found";
		}
	};
};

#endif
