/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raquest.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvilchez <pvilchez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:12:49 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/02 21:30:40 by pvilchez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

class Request
{
	public:
		Request(std::string rawStr);
		~Request();
		
		std::string getType();
		std::string getPath();

	private:
		std::string _type;
		std::string _path;
};

#endif