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

	private:
		std::string _name;
		int _port;
		std::string _hostAddress;
		std::string _errorPages;
		int _maxBodySize;

		std::string _defaultFolder;
		std::string _defaultFile;
		std::map<std::string, std::set<std::string> > routes;

};

#endif