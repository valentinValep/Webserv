/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/14 11:56:12 by vlepille         ###   ########.fr       */
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
	int			clientSocket;
	Server		*server;
	std::string method;
	std::string path;
	std::string protocol;
	std::vector<std::string> headers;
	std::string body;
	std::string raw_data;

	ClientRequest();
	void	parse();
	void	print() const;
};

#endif