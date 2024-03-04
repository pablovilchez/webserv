#include "Location.hpp"

Location::Location() { }

Location::~Location() { }

// SETTERS ---------------------------------
void Location::setAcceptedMethod(const std::string &method) {
	_acceptedMethods.insert(method);
}

void Location::setPath(const std::string &path) {
	_path = path;
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



// GETTERS ---------------------------------
bool Location::isAcceptedMethod(const std::string &method) const {
	return _acceptedMethods.find(method) != _acceptedMethods.end();
}

std::string Location::getPath() const {
	return _path;
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