#include "Config.hpp"

Config::Config() {
	_port.insert(8080);
	_serverName = "default_server";
	_errorPages.insert(std::make_pair(400, "var/www/error/400.html"));
	_maxSize = 1024 * 300;
	Location *newLocation = new Location();
	setLocation(newLocation);
}

Config::Config(const std::string &config) {
	(void)config;
}

Config::~Config() {

}

void Config::printData() {
	std::cout << "Server name: " << _serverName << std::endl;
	std::cout << "Ports:       ";
	for(std::set<int>::iterator it = _port.begin(); it != _port.end(); it++)
		std::cout << *it << "  ";
	std::cout << std::endl;
	std::cout << "Error pages: ";
	std::map<int, std::string>::iterator it;
	for(it = _errorPages.begin(); it != _errorPages.end(); it++)
		std::cout << it->first << "  ";
	std::cout << std::endl;
	std::cout << "Max Size:    " << _maxSize << std::endl;
	for(std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); it++)
		it->printData();
}

void Config::setLocation(Location *location) {
	_locations.push_back(*location);
}

std::set<int> Config::getPort() const {
	return _port;
}

std::string Config::getServerName() const {
	return _serverName;
}

std::string Config::getErrorPage(const int &errorCode) const {
	std::map<int, std::string>::const_iterator it;
	it = _errorPages.find(errorCode);
	if (it != _errorPages.end())
		return it->second;
	return "";
}

int Config::getMaxSize() const {
	return _maxSize;
}

const Location& Config::getLocation(const std::string &location) const {
	std::vector<Location>::const_iterator it;
	for(it = _locations.begin(); it != _locations.end(); it++)
	{
		if(it->getLocation() == location)
			return *it;
	}
	static const Location invalidLocation(0);
	return invalidLocation;
}