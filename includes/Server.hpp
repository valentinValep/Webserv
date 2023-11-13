/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/13 17:15:47 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Route.hpp"
# include "FileParser.hpp"
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <netinet/in.h>

struct SocketInfo {
	int socket;
	time_t lastActivity;
};
class Server {
	private:
		// Attributes
		bool		autoindex;
		int 		port;
		int			methods; // optional if some routes and all these routes have methods
		int			max_body_size; // optional // @TODO convert to long ?
		std::string root;
		std::string index;
		std::map<int, std::string> error_pages;
		std::map<std::string, Route> routes; // optional if methods
		std::vector<std::string> server_names; // optional

		// Parsers
		void		parseAutoindex(fp::Module &mod);
		void		parsePort(fp::Module &mod);
		void		parseMaxBodySize(fp::Module &mod);
		void		parseRoot(fp::Module &mod);
		void		parseIndex(fp::Module &mod);
		void		parseMethods(fp::Module &mod);
		void		parseServerNames(fp::Module &mod);
		void		parseErrorPages(fp::Module &mod);
		void		parseRoutes(fp::Module &mod);


	public:
		// Constructors & Destructor
		Server();
		Server(fp::Module &mod);
		~Server();

		// Getters
		int getPort() const;
		int getMethods() const;
		long getMaxBodySize() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::map<int, std::string> getErrorPages() const;
		std::map<std::string, Route> getRoutes() const;
		std::vector<std::string> getServerNames() const;
		std::vector<SocketInfo>& getClientSockets();

		void		acceptNewConnections();
		void		addClientSocket(int socket);
		void		removeClientSocket(int socket);
		void		updateClientSocketActivity(int socket);
		void		detectInactiveClientSockets();
		void		printActiveSockets();
		void		printListeningSocket();

		//@TODO put in private
		int server_fd;
		struct sockaddr_in address;
		std::vector<SocketInfo> clientSockets;
};

#endif
