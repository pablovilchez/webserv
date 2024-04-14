#include "Location.hpp"

Location::Location() : _directoryListing(false), _rootLocation(false) { }

Location::Location(const Location &other) {
	_location = other._location;
	_acceptedMethods = other._acceptedMethods;
	_root = other._root;
	_index = other._index;
	_directoryListing = other._directoryListing;
	_cgiExtension = other._cgiExtension;
	_return = other._return;
	_rootLocation = other._rootLocation;
}

Location::Location(const std::string &config) : _location("null"), _directoryListing(false), _rootLocation(false) {
	parseConfig(config);
}

Location& Location::operator=(const Location &other) {
	if (this != &other) {
		_location = other._location;
		_acceptedMethods = other._acceptedMethods;
		_root = other._root;
		_index = other._index;
		_directoryListing = other._directoryListing;
		_cgiExtension = other._cgiExtension;
		_return = other._return;
		_rootLocation = other._rootLocation;
	}
	return *this;
}

Location::~Location() {

}

bool loc_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++) {
		if (!std::isspace(line[i])) {
			return line[i] == '#';
		}
	}
	return true;
}

void Location::parseConfig(const std::string &config) {
	std::istringstream stream(config);
	std::string line;
	std::string key;
	std::string value;

	while (std::getline(stream, line)) {
		if (loc_isComment(line)) {
			continue;
		}
		std::istringstream lineStream(line);
		lineStream >> key;
		if (key == "location") {
			if (lineStream >> value) {
				_location = value;
				if (_location == "/")
					_rootLocation = true;
			}
			else
				_location = "null";
		}
		else if (key == "root") {
			if (lineStream >> value)
				_root = value;
			else
				_root = "";
		}
		else if (key == "autoindex") {
			lineStream >> value;
			if(value == "on") {
				_directoryListing = true;
			}
			else {
				_directoryListing = false;
			}
		}
		else if (key == "index") {
			while (lineStream >> value) {
				_index.insert(value);
			}
		}
		else if (key == "allow_methods") {
			while (lineStream >> value) {
				_acceptedMethods.insert(value);
			}
		}
		else if (key == "return") {
			int code;
			lineStream >> code;
			lineStream >> value;
			_return.insert(std::make_pair(code, value));
		}
		else if (key == "cgi_pass") {
			lineStream >> value;
			std::string extension = value;
			lineStream >> value;
			_cgiExtension[extension] = value;
		}
		else if(key == "{") {
			continue;
		}
		else if(key == "}") {
			break;
		}
	}
}

void Location::printData() const {
	std::cout << "Location  " << _location << std::endl;
	std::cout << "{" << std::endl;
	std::cout << "  Accepted methods: ";
	std::set<std::string>::iterator it;
	for(it = _acceptedMethods.begin(); it != _acceptedMethods.end(); it++) {
		std::cout << *it << "  ";
	}
	std::cout << std::endl;
	std::cout << "  Root:  " << _root << std::endl;
	std::cout << "  Index:  ";
	for(it = _index.begin(); it != _index.end(); it++) {
		std::cout << *it << "  ";
	}
	std::cout << std::endl;
	std::cout << "  Directory listing:  " << (_directoryListing ? "ON" : "OFF");
	std::cout << std::endl;
	std::cout << "  CGI extensions:  ";
	std::map<std::string, std::string>::const_iterator it_2;
	for(it_2 = _cgiExtension.begin(); it_2 != _cgiExtension.end(); it_2++) {
		std::cout << it_2->first << "  ";
	}
	std::cout << std::endl;
	std::cout << "  Return:  ";
	std::map<int, std::string>::const_iterator it_3;
	it_3 = _return.begin();
	if(it_3 != _return.end()) {
		std::cout << it_3->first;
	}
	std::cout << std::endl << "}" << std::endl;
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
bool	Location::isIndexFile(std::string& fileName) const {
	return _index.find(fileName) != _index.end();
}

bool	Location::getRootLocation() const {
	return (_rootLocation);
}
