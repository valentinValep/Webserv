/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/22 16:26:46 by vlepille         ###   ########.fr       */
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

class Server {
	private:
		// Attributes
		bool		autoindex;
		int 		port;
		int			methods; // optional if some routes and all these routes have methods @TODO
		long		max_body_size; // optional, if not set, default is -1
		std::string root;
		std::string index;
		std::map<int, std::string> error_pages; // optional
		std::map<std::string, Route> routes; // optional if methods @TODO
		std::vector<std::string> server_names; // optional

		// Parsers
		void	parseAutoindex(fp::Module &mod);
		void	parsePort(fp::Module &mod);
		void	parseMaxBodySize(fp::Module &mod);
		void	parseRoot(fp::Module &mod);
		void	parseIndex(fp::Module &mod);
		void	parseMethods(fp::Module &mod);
		void	parseServerNames(fp::Module &mod);
		void	parseErrorPages(fp::Module &mod);
		void	warningErrorPages();
		void	parseRoutes(fp::Module &mod);


		Server();
	public:
		// Constructors & Destructor
		Server(fp::Module &mod);
		~Server();

		// Getters
		bool							getAutoindex() const;
		int								getPort() const;
		int								getMethods() const;
		long							getMaxBodySize() const;
		std::string						getRoot() const;
		std::string						getIndex() const;
		std::map<int, std::string>		getErrorPages() const;
		std::vector<std::string>		getServerNames() const;

		const Route						*getRoute(const std::string &path) const;
		bool							hasServerName(const std::string& serverName) const;

		//void							updateClientSocketActivity(int socket);
		//void							detectInactiveClientSockets();
		//void							printActiveSockets();
};

#endif
