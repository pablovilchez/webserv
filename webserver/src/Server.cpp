#include "Server.hpp"

Server::Server() { }

Server::Server(const std::string &config, int servNum) : _config(config, servNum) {
	_config.printData();
}

Server::~Server() { }

