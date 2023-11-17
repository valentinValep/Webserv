/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/17 10:57:59 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP

# include "Server.hpp"
# include <string>
# include <vector>
# include <sstream>
# include <iostream>

enum State {
	RECEIVING_METHOD,
	RECEIVING_HEADER,
	RECEIVING_BODY,
	REQUEST_FULLY_RECEIVED,
	ERROR,
};

class ClientRequest {
private:
	class RequestStream: public std::stringstream {
	public:
		RequestStream() {}
		RequestStream(const std::string &data): std::stringstream(data) {}
		RequestStream(const RequestStream &other) {
			//std::cout << "RequestStream copy constructor" << other.str() << std::endl;
			*this << other.str();
		}
		~RequestStream() {}
		RequestStream &operator=(const RequestStream &other) {
			if (this != &other) {
				this->clear();
				*this << other.str();
			}
			return (*this);
		}
	};

	in_port_t							_port;
	int									_errorCode;
	int									_clientSocket;
	int									_method;
	Server								*_server;
	State								_state;
	std::string							_path;
	std::string							_protocol;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	RequestStream						_raw_data;

	void	findFirstServer(std::vector<Server> &servers);
	void	findFinalServer(std::vector<Server> &servers);
	void	parseMethodLine(const std::string line);
	void	parseHeaderLine(const std::string line);
	bool	needBody() const;
	void	parseBodyLine(const std::string line);

public:
	ClientRequest();
	ClientRequest(int fd, in_port_t port);

	void		setError(int errorCode);
	void		print() const;

	void		operator<<(const std::string &data);
	void		parse(std::vector<Server> &servers);
	bool		isFullyReceived() const;
	bool		isError() const;
	std::string	getHeader(const std::string &key) const;
	void		reset();

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