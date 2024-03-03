/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 01:03:19 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/03 11:45:43 by pvilchez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"

Response::Response(const Request &request)
{
	_type = request.getType();
}

Response::~Response() { }

void Response::printData()
{
	std::cout << "Response header:  " << _header << std::endl;
	std::cout << "Response type:    " << _type << std::endl;
	std::cout << "Response content: " << _content << std::endl;
	std::cout << std::endl;
}

std::string Response::getHeader()
{
	return _header;
}

std::string Response::getType()
{
	return _type;
}

std::string Response::getContent()
{
	return _content;
}