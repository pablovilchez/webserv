#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <set>
# include <map>
# include "Location.hpp"

class Config
{
	public:
		typedef std::map<int, std::string> ErrorPageMap;

		Config();
		Config(const std::string &config);
		~Config();

		void printData();

		void setLocation(Location *location);

		int getPort() const;
		std::string getServerName() const;
		std::string getErrorPage(const int &errorCode) const;
		int getMaxSize() const;
		const Location &getLocation(const std::string &location) const;

	private:
		int						_port;
		std::string				_serverName;
		ErrorPageMap			_errorPages;
		int						_maxSize;
		std::vector<Location>	_locations;
};

#endif