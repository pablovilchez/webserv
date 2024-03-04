/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:18:22 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/04 17:34:43 by gkrusta          ###   ########.fr       */
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
	size_t	pos = 0;
	size_t	end = _raw.find("\r\n");

	// parse request line
	if (end != std::string::npos) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	space1 = line.find(' ');
		size_t	space2 = line.find(' ', space1 + 1);

		if (space1 != std::string::npos || space2 != std::string::npos) {
			_method = line.substr(0, space1);
			_path = line.substr(space1 + 1, space2 - space1 - 1);
			_version = line.substr(space2 + 1);
		}
			pos = end + 2; // Move to the next position after "\r\n"
	}

	// parse header
	while ((end = _raw.find("\r\n")) != std::string::npos && pos < _raw.size()) {
		std::string	line = _raw.substr(pos, end - pos);
		    std::cout << "Line: " << line << std::endl;
    		std::cout << "Pos: " << pos << ", End: " << end << ", Size: " << _raw.size() << std::endl;
		pos = end + 2;
		size_t	colPos = line.find(':');
		if (colPos != std::string::npos) {
			std::string	key = line.substr(0, colPos);
			std::string	value = line.substr(colPos + 2);
			if (key == "Host")
				_host = value;
			else if (key == "Content-Type")
				_contentType = value;
			else if (key == "Content-Length")
				_contentLength = atoi(value.c_str());
		}
		else
			break ;
	}
/* 	size_t dotPos = _path.rfind('.');
	if (dotPos != std::string::npos) 
		_contentType = _path.substr(dotPos + 1); */
	if (_contentLength > 0) {
		_body = _raw.substr(pos, _contentLength);
	}
}

void Request::printData()
{
	std::cout << "Request _method:    " << _method << std::endl;
	std::cout << "Request _path:    " << _path << std::endl;
	std::cout << "Request _version:    " << _version << std::endl;
	std::cout << "Request _host:    " << _host << std::endl;
	std::cout << "Request _contentType:    " << _contentType << std::endl;
	std::cout << "Request _contentLength:    " << _contentLength << std::endl;
	std::cout << "Request _body:    " << _body << std::endl;

	std::cout << std::endl;
}

/* std::string Request::getHeader() const
{
	return _header;
}

std::string Request::getMethod() const
{
	return _method;
}

std::string Request::getPath() const
{
	return _path;
}

std::string Request::getExtension() const
{
	return _extension;
} */