/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 13:12:30 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 14:28:14 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP

# include <string>
# include <vector>
# include <sstream>
# include <iostream>

class ClientRequest {
public:
	std::string method;

	ClientRequest(const std::string& request);
	void print() const;
	
};

#endif