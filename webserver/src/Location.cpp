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

Location::Location(const std::string &path) {
	if(path == "null")
		_location = "";
	else
		_location = path;
}

Location::~Location() { }

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