/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/21 15:20:52 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP

# include "Server.hpp"
# include "Body.hpp"
# include <string>
# include <vector>
# include <sstream>
# include <iostream>

enum State {
	RECEIVING_METHOD,
	RECEIVING_HEADER,
	RECEIVING_BODY,
	REQUEST_FULLY_RECEIVED,
	CLOSED,
	ERROR,
};

class ClientRequest {
private:
	class RequestStream: public std::stringstream {
	public:
		RequestStream() {}
		RequestStream(const std::string &data): std::stringstream(data) {}
		RequestStream(const RequestStream &other): std::basic_ios<char>(), std::stringstream() {
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

	bool								_cgiRequest;
	in_port_t							_port;
	int									_errorCode;
	int									_clientSocket;
	int									_method;
	State								_state;
	Server								*_server;
	std::string							_path;
	std::string							_protocol;
	std::map<std::string, std::string>	_headers;
	Body								_body;
	RequestStream						_raw_data;

	void	detectCgi();
	void	findFirstServer(std::vector<Server> &servers);
	void	findFinalServer(std::vector<Server> &servers);
	void	parseMethodLine(const std::string line);
	void	parseHeaderLine(const std::string line);
	bool	needBody();
	void	parseBodyLine(const std::string line);

public:
	ClientRequest();
	ClientRequest(int fd, in_port_t port);

	void		setError(int errorCode);
	void		print() const;
	void		short_print() const;

	void		operator<<(const std::string &data);
	void		parse(std::vector<Server> &servers);
	bool		isFullyReceived() const;
	bool		isError() const;
	bool		isClosed() const;
	std::string	getHeader(const std::string &key) const;
	void		reset();
	void		close();

	// Getters
	in_port_t							getPort() const;
	int									getErrorCode() const;
	int									getClientSocket() const;
	Server								*getServer() const;
	int									getMethod() const;
	std::string							getPath() const;
	std::string							getProtocol() const;
	std::map<std::string, std::string>	getHeaders() const;
	bool								isCgiRequest() const;
};

#endif
