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
		int _port;
		std::string _host;
		std::string _name;
		std::string _root;
		std::string _index;
		//default error pages
		int _maxBodySize;
		std::map<std::string, std::set<std::string>> routes;

};

#endif