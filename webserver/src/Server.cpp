#include "Server.hpp"

Server::Server() { }

Server::Server(const std::string &config) : _config(config) {
	std::cout << "Server created. Unparsed config: " << std::endl;
	std::cout << config << std::endl;
}

Server::~Server() { }

