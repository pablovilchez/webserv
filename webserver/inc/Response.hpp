/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 01:03:16 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/03 12:13:25 by pvilchez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include "Request.hpp"

class Response
{
	public:
		Response(const Request &request);
		~Response();

		void printData() const;

		std::string getHeader() const;
		std::string getType() const;
		std::string getContent() const;
		std::string getPath() const;
		
	private:
		std::string _type;
		std::string _header;
		std::string _content;
		std::string _path;
};

#endif