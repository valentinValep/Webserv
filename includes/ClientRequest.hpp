/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 14:03:30 by chmadran         ###   ########.fr       */
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
	std::string method;
	std::string path;
	std::string protocol;
	std::vector<std::string> headers;
	std::string body;
	std::string raw_data;
	State 		state;
	int			_clientSocket;
	
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