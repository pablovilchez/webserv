#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "libraries.hpp"
#include "Location.hpp"

class Config
{
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		Config(const std::string &config, int servNum);
		~Config();

		void defaultConfig(int servNum);
		void parseConfig(const std::string &config, int servNum);
		void printData();

		void setLocation(const Location *location);

		std::set<int> getPort() const;
		std::string getServerName() const;
		std::string getErrorPage(const int &errorCode) const;
		int getMaxSize() const;
		const Location &getLocation(const std::string &location) const;

	private:
		std::set<int>					_port;
		std::string						_serverName;
		std::map<int, std::string>		_errorPages;
		int								_maxSize;
		std::vector<const Location*>	_locations;
};

#endif
