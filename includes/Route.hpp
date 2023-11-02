/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/02 16:46:08 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "Route.hpp"
# include <iostream>
# include <string>
# include <vector>

# define GET 1
# define POST 2
# define DELETE 4

# define ACCEPTED_CGI ["php"] // @TODO choose the right one

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
	public:
		// Constructor & Destructor
		Route();
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
