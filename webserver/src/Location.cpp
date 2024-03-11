#include "Location.hpp"

Location::Location() {
	_location = "/default";
	_acceptedMethods.insert("GET");
	_acceptedMethods.insert("POST");
	_acceptedMethods.insert("DELETE");
	_root = "var/default";
	_index.insert("index.html");
	_directoryListing = false;
}

Location::Location(const std::string &data) {
	parseData(data);
}

Location::Location(int error) {
	_location = "null";
}

Location::~Location() { }

void Location::parseData(const std::string &data) {
	(void)data;
}

void Location::printData() const {
	std::cout << "Location:  " << _location << std::endl;
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

// Check if the file is in the index set
bool	Location::isIndexFile(const std::string& fileName) const {
	return _index.find(fileName) != _index.end();
}