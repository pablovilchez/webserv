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