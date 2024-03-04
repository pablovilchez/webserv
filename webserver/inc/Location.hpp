#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <set>
# include <map>
# include <vector>

class Location
{
	public:
		Location();
		~Location();

		void setAcceptedMethod(const std::string &method);
		void setPath(const std::string &path);
		void setRoot(const std::string &rootPath);
		void setIndex(const std::string &indexPath);
		void setDirectoryListing(const bool &value);
		void setCgiExtension(const std::string &extension, const std::string &executable);

		bool isAcceptedMethod(const std::string &method) const;
		std::string getPath() const;
		std::string getRoot() const;
		std::set<std::string> getIndex() const;
		bool getDirectoryListing() const;

	private:
		std::set<std::string>				_acceptedMethods;
		std::string							_path;
		std::string							_root;
		std::set<std::string>				_index;
		bool								_directoryListing;
		std::map<std::string, std::string>	_cgiExtension;

};

#endif