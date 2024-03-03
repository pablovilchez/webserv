/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 01:03:16 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/03 01:48:27 by pvilchez         ###   ########.fr       */
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

		void printData();

		std::string getHeader();
		std::string getType();
		std::string getContent();
		
	private:
		std::string _header;
		std::string _type;
		std::string _content;
};

#endif