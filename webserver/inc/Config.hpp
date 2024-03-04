#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <set>
# include <map>

class Config
{
	public:
		Config();
		~Config();

		void printData();

		typedef std::map<int, std::string> ErrorPageMap;
		std::string getErrorPage(int errorCode);

	private:
		int _port;
		std::string _serverName;
		ErrorPageMap _errorPages;
		std::string _hostAddress;
		std::string _errorPages;
		int _maxBodySize;

		std::string _defaultFolder;
		std::string _defaultFile;
		std::map<std::string, std::set<std::string> > routes;

};

#endif