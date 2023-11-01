/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:13:25 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 14:11:03 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ClientRequest.hpp"

ClientRequest::ClientRequest(const std::string& request) {
	// Parse the request
	std::istringstream iss(request);
	iss >> method;
}

void ClientRequest::print() const {
	std::cout << "Method: " << method << std::endl;
}