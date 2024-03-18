#ifndef SERVER_HPP
#define SERVER_HPP

#include "libraries.hpp"
#include "Location.hpp"

class Server
{
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		Server(const std::string &serverConfig, int servNum);
		~Server();

		void defaultServer(int servNum);
		void parseServer(const std::string &serverConfig, int servNum);
		void printData();

		void setLocation(Location location);

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
		std::vector<Location>			_locations;
};

#endif
