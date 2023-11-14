/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 14:33:39 by vlepille         ###   ########.fr       */
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

	int			errorCode; // if 0, no error
	int			_clientSocket;
	Server		*server;
	int			method;
	std::string path;
	std::string protocol;
	std::vector<std::string> headers;
	std::string body;
	std::string raw_data;
	State		state;

	//headerstuff
	std::string	header;
	int			headerLen;

	ClientRequest();
	ClientRequest(int fd);

	void	parse();
	void	print() const;

	void	setState(State newState);
	void	setHeaderInfos(std::string _header, int _headerLen);
};

#endif