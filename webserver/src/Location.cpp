#include "Location.hpp"

Location::Location() : _directoryListing(false) {
	defaultConfig();
}

void Location::defaultConfig() {
	if(_location.empty())
		_location= "/default";
	if(_acceptedMethods.empty())
	{
		_acceptedMethods.insert("GET");
		_acceptedMethods.insert("POST");
		_acceptedMethods.insert("DELETE");
	}
	if(_root.empty())
		_root = "var/default";
	if(_index.empty())
		_index.insert("index.html");
}

bool isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++)
	{
		if (!std::isspace(line[i]))
			return line[i] == '#';
	}
	return true;
}

void Location::parseConfig(const std::string &config) {
	std::istringstream stream(config);
	std::string line;
	std::string key;
	std::string value;

	while (std::getline(stream, line))
	{
		if (isComment(line))
			continue;
		std::istringstream lineStream(line);
		lineStream >> key;
		if (key == "location")
		{
			lineStream >> value;
			_location = value;
		}
		if (key == "root")
		{
			lineStream >> value;
			_root = value;
		}
		if (key == "autoindex")
		{
			lineStream >> value;
			if(value == "on")
				_directoryListing = true;
			else
				_directoryListing = false;
		}
		else if (key == "index")
		{
			while (lineStream >> value)
				_index.insert(value);
		}
		else if (key == "allow_methods")
		{
			while (lineStream >> value)
				_acceptedMethods.insert(value);
		}
		else if (key == "return")
		{
			int code;
			lineStream >> code;
			lineStream >> value;
			_return.insert(std::make_pair(code, value));
		}
		else if (key == "cgi_extension")
		{
			lineStream >> value;
			std::string extension = value;
			lineStream >> value;
			_cgiExtension[extension] = value;
		}
		else if(key == "}")
			break;
		else
		{
			std::cerr << "Error: Unknown key: " << key << std::endl;
		}
	}
	defaultConfig();
}

Location::Location(const std::string &config) : _directoryListing(false) {
	parseConfig(config);
}

Location::~Location() { }

void Location::printData() const {
	std::cout << "Location  " << _location << std::endl;
	std::cout << "{" << std::endl;
	std::cout << "  Accepted methods: ";
	std::set<std::string>::iterator it;
	for(it = _acceptedMethods.begin(); it != _acceptedMethods.end(); it++)
		std::cout << *it << "  ";
	std::cout << std::endl;
	std::cout << "  Root:  " << _root << std::endl;
	std::cout << "  Index:  ";
	for(it = _index.begin(); it != _index.end(); it++)
		std::cout << *it << "  ";
	std::cout << std::endl;
	std::cout << "  Directory listing:  " << (_directoryListing ? "ON" : "OFF");
	std::cout << std::endl;
	std::cout << "  CGI extensions:  ";
	std::map<std::string, std::string>::const_iterator it_2;
	for(it_2 = _cgiExtension.begin(); it_2 != _cgiExtension.end(); it_2++)
		std::cout << it_2->first << "  ";
	std::cout << std::endl;
	std::cout << "  Return:  ";
	std::map<int, std::string>::const_iterator it_3;
	it_3 = _return.begin();
	if(it_3 != _return.end())
		std::cout << it_3->first;
	std::cout << std::endl << "}" << std::endl;
}

/* 
// SETTERS ---------------------------------
void Location::setLocation(const std::string &path) {
	_location = path;
}

void Location::setAcceptedMethod(const std::string &method) {
	_acceptedMethods.insert(method);
}

void Location::setRoot(const std::string &rootPath) {
	_root = rootPath;
}

void Location::setIndex(const std::string &index) {
	_index.insert(index);
}

void Location::setDirectoryListing(const bool &value) {
	_directoryListing = value;
}

void Location::setCgiExtension(const std::string &extension, const std::string &executable) {
	_cgiExtension[extension] = executable;
}

void Location::setReturn(const int &httpCode, const std::string &redir) {
	_return[httpCode] = redir;
}
 */


// GETTERS ---------------------------------
std::string Location::getLocation() const {
	return _location;
}

bool Location::isAcceptedMethod(const std::string &method) const {
	return _acceptedMethods.find(method) != _acceptedMethods.end();
}

std::string Location::getRoot() const {
	return _root;
}

std::set<std::string> Location::getIndex() const {
	return _index;
}

bool Location::getDirectoryListing() const {
	return _directoryListing;
}

std::map<int, std::string> Location::getReturn() const {
	return _return;
}

std::map<std::string, std::string> Location::getCgiExtension() const {
	return _cgiExtension;
}