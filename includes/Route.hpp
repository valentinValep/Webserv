/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/21 11:59:31 by vlepille         ###   ########.fr       */
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
# define ACCEPTED_CGI (std::string [ACCEPTED_CGI_TABSIZE]){".py"} // @TODO choose the right one

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
		// @TODO if there is a redirect can we have a cgi ? Same with upload ?

		bool		_autoindex_set;
		bool		_methods_set;
		bool		_redirect_set;
		bool		_index_set;
		bool		_root_set;
		bool		_cgi_set;
		bool		_upload_set;

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
		bool		getAutoindex() const;
		int			getMethods() const;
		int			getRedirectType() const;
		std::string	getRedirect() const;
		std::string	getIndex() const;
		std::string	getRoot() const;
		std::string	getCgiExtension() const;
		std::string	getCgiPath() const;
		std::string	getUploadPath() const;

		bool		hasAutoindex() const;
		bool		hasMethods() const;
		bool		hasRedirect() const;
		bool		hasIndex() const;
		bool		hasRoot() const;
		bool		hasCgi() const;
		bool		hasUpload() const;
};

#endif
