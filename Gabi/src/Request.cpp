/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:18:22 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/03 11:53:24 by pvilchez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <iostream>

Request::Request(const std::string &raw) : _raw(raw)
{
	parseContent();
}

Request::~Request() { }

std::string Request::getRaw() const
{
	return _raw;
}

void Request::parseContent()
{
	std::string::size_type i = 0;
	while (i < _raw.length())
	{
		while (_raw[i] != '\n' && _raw[i] != '\0')
		{
        	_header += _raw[i];
			i++;
		}
		while (_raw[i] != '\0')
		{
        	_body += _raw[i];
			i++;
		}
    }
	i = 0;
	while (i < _header.length())
	{
		while (_header[i] != ' ' && _header[i] != '\0')
		{
			_type += _header[i];
			i++;
		}
		i++;
		while (_header[i] != ' ' && _header[i] != '\0')
		{
			_path += _header[i];
			i++;
		}
		break;
    }
	size_t dotPos = _path.rfind('.');
    if (dotPos != std::string::npos) 
		_extension = _path.substr(dotPos + 1);
}

void Request::printData()
{
	std::cout << "Request type:    " << _type << std::endl;
	std::cout << "Request path:    " << _path << std::endl;
	std::cout << "Request exte:    " << _extension << std::endl;
	std::cout << std::endl;
}

std::string Request::getHeader() const
{
	return _header;
}

std::string Request::getType() const
{
	return _type;
}

std::string Request::getPath() const
{
	return _path;
}

std::string Request::getExtension() const
{
	return _extension;
}