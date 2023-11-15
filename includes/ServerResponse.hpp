/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:52:18 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/15 20:46:00 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERRESPONSE_HPP
# define SERVERRESPONSE_HPP

# include "ClientRequest.hpp"
# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <fstream>
# include <iostream>
# include <algorithm>

class ServerResponse {
	private:
		bool								_autoindex;
		int									_error_code;
		int									_method;
		int									_client_socket;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		std::string							_root;
		std::string							_index;
		std::map<int, std::string>			_error_pages;
		int									_redirect_type; // @TODO learn about it
		std::string							_redirect; // @TODO learn about it
		std::string							_cgi_extension; // @TODO learn about it
		std::string							_cgi_path; // @TODO learn about it
		std::string							_upload_path; // @TODO learn about it

		std::string		_getGenericErrorPage(void) const;
	public:
		ServerResponse();

		void		prepare(const ClientRequest& request);
		void		process();
		std::string	readFileContent(const std::string& filePath);
		void		sendHttpResponse(int clientSocket, const std::string& content, const std::string& contentType);
		void		sendHttpResponseCSS(int clientSocket, const std::string& content);
};

#endif
