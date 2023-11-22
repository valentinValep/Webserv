/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:52:18 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/22 14:11:22 by chmadran         ###   ########.fr       */
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
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <sstream>
# include <cstdio>

# define HTTPVERSION "1.1"
# define MAX_FILE_SIZE 100000
# define UPLOAD_LOCATION "./www/var/uploads"


class ServerResponse {
	private:
		in_port_t							_port;
		bool								_autoindex;
		int									_error_code;
		int									_method;
		int									_client_socket;
		std::string							_path;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_root;
		std::string							_index;
		std::map<int, std::string>			_error_pages;
		int									_redirect_type; // @TODO learn about it
		std::string							_redirect; // @TODO learn about it
		
		//CGI
		bool								_cgi_request;
		std::string							_cgi_extension;
		std::string							_cgi_path;

		//Upload
		bool								_file_upload;
		std::string							_upload_path; // @TODO learn about it
		std::string							_boundary;
		std::string							_file_name;
		std::string							_file_body;
		
		void			setError(int errorCode);
		std::string		_getGenericErrorPage(int) const;
		void			_sendErrorPage(int);
		void			_sendAutoIndexed(std::string const &);
	
	
	public:
		ServerResponse();

		void		prepare(const ClientRequest& request);
		void		process();
		std::string	readFileContent(std::string const &filePath, std::string &mimeType);
		void		sendHttpResponse(int clientSocket, const std::string& content, const std::string& contentType);
		void		sendHttpResponseCSS(int clientSocket, const std::string& content);
		void		sendCGIResponse(int clientSocket, const std::string& content, const std::string& contentType);

		// Upload
		void			setUpload();
		std::string		extractBoundary();
		std::string		extractFileBody(size_t filenameEndPos);
		int				createWriteFile();
		void			sendUploadResponse(int clientSocket, const std::string& content, const std::string& contentType);

		// Getters
		int			getMethod() const;
		int			getPort() const;
		int			getContentLength() const;
		std::string	getCgiPath() const;
		std::string	getPath() const;
		std::string	getRoot() const;
		std::string	getContentType() const;
};

#endif
