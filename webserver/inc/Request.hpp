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
		
		std::string getRaw() const;
		std::string getHeader() const;
		std::string getType() const;
		std::string getPath() const;
		std::string getExtension() const;

	private:
		std::string _raw;
		std::string _header;
		std::string _body;
		std::string _type;
		std::string _path;
		std::string _extension;
};

#endif