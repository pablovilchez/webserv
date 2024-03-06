/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:12:49 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/06 11:41:01 by gkrusta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <fstream>
#include <unistd.h>
#include <header.hpp>
class Request
{
	public:
		Request(const std::string &raw, int clientSocket);
		~Request();

		void	parseContent(int clientSocket);
		void	parseBody(int clientSocket);
		void	printData();
		
		std::string getRaw() const;
/* 		std::string getHeader() const;
		std::string getMethod() const;
		std::string getPath() const;
		std::string getExtension() const; */

	private:
		std::string _raw;
		std::string _method;
		std::string _path;
		std::string _version;
		std::string _host;
		std::string _contentType;
		size_t			 _contentLength;
		std::string _body;
		int			_number;
};

#endif