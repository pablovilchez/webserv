#include "Response.hpp"

Response::Response(const Request &request)
{
	_type = request.getType();
}

Response::~Response() { }

void Response::printData() const
{
	std::cout << "Response header:  " << _header << std::endl;
	std::cout << "Response type:    " << _type << std::endl;
	std::cout << "Response content: " << _content << std::endl;
	std::cout << std::endl;
}

std::string Response::getHeader() const
{
	return _header;
}

std::string Response::getType() const
{
	return _type;
}

std::string Response::getContent() const
{
	return _content;
}
