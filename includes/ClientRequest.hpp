/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/15 15:11:31 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP

# include "Server.hpp"
# include <string>
# include <vector>
# include <sstream>
# include <iostream>

class ClientRequest {
public:
	enum State {
		HEADER_FULLY_RECEIVED,
		HEADER_NOT_FULLY_RECEIVED,
		BODY_FULLY_RECEIVED,
		BODY_NOT_FULLY_RECEIVED,
		REQUEST_FULLY_RECEIVED,
	};

	int									errorCode; // if 0, no error
	int									_clientSocket;
	Server								*server;
	int									method;
	State								state;
	std::string							path;
	std::string							protocol;
	std::map<std::string, std::string>	headers;
	std::string							body;
	std::string							raw_data;

	ClientRequest();
	ClientRequest(int fd);

	void	parseHeader(std::vector<Server> &servers);
	void	parseBody();
	void	print() const;

	void	setState(State newState);
	void	setBodyState();
	void	findServer(std::vector<Server> &servers);

	// Getters
	int									getErrorCode() const;
	int									getClientSocket() const;
	Server								*getServer() const;
	int									getMethod() const;
	std::string							getPath() const;
	std::string							getProtocol() const;
	std::map<std::string, std::string>	getHeaders() const;
};

#endif