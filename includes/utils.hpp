/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 19:21:08 by vlepille          #+#    #+#             */
/*   Updated: 2023/11/28 14:33:33 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>

int			anti_overflow_atoi(const char *input, int *result);
bool		containsEmptyLine(const std::string& data);
std::string	trimTrailingSlashes(std::string path);
