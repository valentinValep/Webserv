/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 14:41:58 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 14:41:59 by chmadran         ###   ########.fr       */
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