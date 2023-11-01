/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 11:16:31 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "ConfigSettings.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

class Server {
public:
    Server(const ConfigSettings& settings);
    void start();

private:
    ConfigSettings settings;
};

#endif
