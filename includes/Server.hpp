/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 12:13:58 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include "ConfigSettings.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <cstring>
# include <fstream>
# include <iostream>
class Server {
public:
	Server(const ConfigSettings& settings);
	void start();

private:
	ConfigSettings settings;
	int server_fd;
	struct sockaddr_in address;
	void setupNetwork();
	void handleClientRequest(int clientSocket);
	std::string readFileContent(const std::string& filePath);
	void sendHttpResponse(int clientSocket, const std::string& content);
};

#endif
