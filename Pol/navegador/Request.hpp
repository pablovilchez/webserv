/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:12:49 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/03 01:44:33 by pvilchez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

class Request
{
	public:
		Request(const std::string &raw);
		~Request();

		void parseContent();
		void printData();
		
		std::string getRaw();
		std::string getHeader();
		std::string getType();
		std::string getPath();
		std::string getExtension();

	private:
		std::string _raw;
		std::string _header;
		std::string _body;
		std::string _type;
		std::string _path;
		std::string _extension;
};

#endif