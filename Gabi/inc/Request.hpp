/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:12:49 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/11 17:08:18 by gkrusta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <header.hpp>

#include "Location.hpp"
#include "Config.hpp"

class Request
{
	public:
		Request(const std::string &raw, int clientSocket);
		~Request();

		void	handleRequest(int clientSocket);
		void	handleGetMethod(std::string &fileToOpen);
		void	handlePostMethod();
		void	handleDeleteMethod(std::string &fileToOpen);

		std::string	extractPathFromUrl(std::string& url);
		bool	validateRequest(const std::string& method);
		void	parseHeader(int clientSocket);
		void	parseBody(const char *buf, int bytesReceived);
		void	printData();
		void	captureFileName(std::string receivedData);
		bool	fileExtension();
		void	setStatus(const std::string &status);
		std::string getPath() const;
		std::string getRaw() const;
		std::string getResponseHeader() const;
		std::string getResponseBody() const;

/* 		std::string getHeader() const;
		std::string getMethod() const;
		std::string getExtension() const; */

	private:
		std::string	_raw;
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::string	_host;
		std::string	_contentType;
		std::string	_fileName;
		size_t		_contentLength;
		std::vector<char>	_body;
		std::string	_boundary;
		//size_t	_bodySize;
		std::string	_status;
		//Server	_server;
		Location	*_location;
		Config		_config;
		std::string	_repsonseHeader;
		std::string	_repsonseBody;
		bool		_done;
};

#endif