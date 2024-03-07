#ifndef CONFIG_HPP
# define CONFIG_HPP

# ifndef DEBUG
#  define DEBUG false
# endif

# include <iostream>
# include <vector>
# include <set>
# include <map>
# include <string>
# include <sstream>
# include <cstdlib>
# include "Location.hpp"

class Config
{
	public:
		Config();
		Config(const std::string &config);
		~Config();

		void defaultConfig();
		void parseConfig(const std::string &config);
		void printData();

		void setLocation(Location *location);

		std::set<int> getPort() const;
		std::string getServerName() const;
		std::string getErrorPage(const int &errorCode) const;
		int getMaxSize() const;
		const Location &getLocation(const std::string &location) const;

	private:
		std::set<int>				_port;
		std::string					_serverName;
		std::map<int, std::string>	_errorPages;
		int							_maxSize;
		std::vector<Location>		_locations;
};

#endif