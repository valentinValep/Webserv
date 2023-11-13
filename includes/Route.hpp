/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/13 13:37:16 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "Route.hpp"
# include "FileParser.hpp"
# include <iostream>
# include <string>
# include <vector>

# define ACCEPTED_CGI_TABSIZE 1
# define ACCEPTED_CGI (std::string [ACCEPTED_CGI_TABSIZE]){".php"} // @TODO choose the right one

class Route {
	private:
		// Attributes
		bool		autoindex;
		int			methods;
		int			redirect_type; // 301 or 302
		std::string	redirect; // if redirect_type
		std::string	index; // ? "if request is a directory"
		std::string	root;
		std::string	cgi_extension; // one to implement
		std::string	cgi_path; // if cgi_extension
		std::string	upload_path;

		// @TODO Define a directory or a file from where the file should be searched (for example, if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is /tmp/www/pouic/toto/pouet).
		void	parseMethods(fp::Module &mod);
		void	parseRoot(fp::Module &mod);
		void	parseIndex(fp::Module &mod);
		void	parseAutoindex(fp::Module &mod);
		void	parseCgiExtension(fp::Module &mod);
		void	parseCgiPath(fp::Module &mod);
		void	parseUploadPath(fp::Module &mod);
		void	parseRedirect(fp::Module &mod);
	public:
		// Constructor & Destructor
		Route();
		Route(fp::Module &mod);
		~Route();

		// Getters
		bool getAutoindex() const;
		int getMethods() const;
		int getRedirectType() const;
		std::string getRedirect() const;
		std::string getIndex() const;
		std::string getRoot() const;
		std::string getCgiExtension() const;
		std::string getCgiPath() const;
		std::string getUploadPath() const;
};

#endif
