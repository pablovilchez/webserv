#include "Config.hpp"

Config::Config() {
	_port = 8080;
	_serverName = "default_server";
	//_errorPages = {{400, "var/www/error/400.html"}};
	Location *newLocation = new Location();
	setLocation(newLocation);
}

Config::Config(const std::string &config) {
	(void)config;
}

Config::~Config() {

}

void Config::printData() {

}

void Config::setLocation(Location *location) {
	_locations.push_back(*location);
}

int Config::getPort() const {
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
	static const Location invalidLocation("null");
	return invalidLocation;
}