/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/08 19:38:35 by vlepille         ###   ########.fr       */
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

# define GET 1
# define POST 2
# define DELETE 4

class Server {
	private:
		// Attributes
		int 		port;
		int			methods; // optional if some routes and all these routes have methods
		int			max_body_size; // optional // @TODO convert to long ?
		std::string root;
		std::string index;
		std::map<int, std::string> error_pages;
		std::map<std::string, Route> routes; // optional if methods
		std::vector<std::string> server_names; // optional

		void		parsePort(fp::Module &mod);
		void		parseMaxBodySize(fp::Module &mod);
		void		parseRoot(fp::Module &mod);
		void		parseIndex(fp::Module &mod);
		void		parseMethods(fp::Module &mod);
		void		parseServerNames(fp::Module &mod);
		void		parseErrorPages(fp::Module &mod);
		void		parseRoutes(fp::Module &mod);
	public:
		// Constructor & Destructor
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
};

#endif
