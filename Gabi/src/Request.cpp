/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:18:22 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/11 17:53:17 by gkrusta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <sys/types.h>
Request::Request(const std::string &raw, int clientSocket) : _raw(raw)
{
	_method = "";
	_path = "";
	_version = "";
	_host = "";
	_contentType = "";
	_contentLength = 0;
	_fileName = "";
	_status = "";
	_boundary = "";
	_repsonseHeader = "";
	_repsonseBody = "";
	_done = false;
	handleRequest(clientSocket);
}

Request::~Request() { }

std::string Request::getRaw() const
{
	return _raw;
}

void	Request::captureFileName(std::string receivedData) {
	size_t filenamePos = receivedData.find("filename=");

	if (filenamePos != std::string::npos) {
		size_t quoteStart = receivedData.find("\"", filenamePos);
		size_t quoteEnd = receivedData.find("\"", quoteStart + 1);
		if (quoteStart != std::string::npos && quoteEnd != std::string::npos) {
			if (quoteEnd - quoteStart == 1)
				_fileName = "default";
			else
				_fileName = receivedData.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
		}
	}
}

bool	Request::fileExtension() {
	if (_contentType.find("image/jpg") != std::string::npos)
		return true;
	else if (_contentType.find("image/png") != std::string::npos)
		return true;
	else if (_contentType.find("txt") != std::string::npos)
		return true;
	else {
		std::cerr << "Unsupported content type." << std::endl;
		return false;
		// error code int
	}
}

void	Request::parseBody(const char *buf, int bytesReceived) {
	static int	i = 0;
	std::string boundary = "\r\n\r\n";
	std::string receivedData(buf, bytesReceived);
	size_t boundaryPos = receivedData.find(boundary);

	if (_contentLength == 0) {
		std::cerr << "Content-Length header is missing or zero." << std::endl;
		return;
	}
	if (_body.empty())
		_body.reserve(_contentLength);
	if (i == 0 && boundaryPos != std::string::npos) {
		captureFileName(receivedData);
		_body.insert(_body.end(), receivedData.begin() + boundaryPos + boundary.size(), receivedData.end());
		i = 1;
	} else
		_body.insert(_body.end(), buf, buf + bytesReceived);

	// Save the file with the appropriate extension
	if (receivedData.find(_boundary + "--") != std::string::npos) {
		if (fileExtension() == true) {
			std::ofstream outputFile(_fileName, std::ios::binary);
			if (outputFile.is_open()) {
				outputFile.write(_body.data(), _body.size());
				outputFile.close();
			} else
				std::cerr << "Error opening file for writing." << std::endl;
		}
		//_body.clear();
	}
}

void Request::parseHeader(int clientSocket)
{
	size_t	pos = 0;
	size_t	end = _raw.find("\r\n");


	if (end != std::string::npos) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	space1 = line.find(' ');
		size_t	space2 = line.find(' ', space1 + 1);

		if (space1 != std::string::npos || space2 != std::string::npos) {
			_method = line.substr(0, space1);
			_path = line.substr(space1 + 1, space2 - space1 - 1);
			_version = line.substr(space2 + 1);
		}
		pos = end + 2;
	}
	if (_method != "GET" || _method != "POST" || _method != "DELETE" ||
		_path[0] != '/' || _version != "HTTP/1.1")
			return (setStatus("400 Bad Request"));
	while ((end = _raw.find("\r\n", pos)) != std::string::npos && pos < _raw.size()) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	colPos = line.find(':');
		if (colPos != std::string::npos) {
			std::string	key = line.substr(0, colPos);
			std::string	value = line.substr(colPos + 2);
			if (key == "Host")
				_host = value;
			else if (key == "Content-Length")
				_contentLength = atoi(value.c_str());
			else if (key == "Content-Type") {
				_contentType = value;
				size_t bPos = value.find("boundary=");
				if (bPos != std::string::npos) {
					_boundary = value.substr(bPos + 9);
				}
			}
		}
		pos = end + 2;
	}
	size_t expectHeaderPos = _raw.find("Expect: 100-continue");
	if (expectHeaderPos != std::string::npos) {

		bool readyToAcceptPayload = true;

		if (readyToAcceptPayload) {
			std::string continueResponse = "HTTP/1.1 100 Continue\r\n\r\n";
			send(clientSocket, continueResponse.c_str(), continueResponse.size(), 0);
		} else {
			std::string finalResponse = "HTTP/1.1 400 Bad Request\r\n\r\n";
			send(clientSocket, finalResponse.c_str(), finalResponse.size(), 0);
			// Close the connection
		}
	}
}

void	Request::buildHeader() {
	_repsonseHeader = "HTTP/1.1 " + _status + "\r\n";
	_repsonseHeader += "Content-Type: " + _contentType + "\r\n";
	_repsonseHeader += "Content-Length: " + _contentLength + "\r\n";
	_repsonseHeader += "\r\n"; 
}

void	Request::buildResponse(std::string &fileToOpen) {
	std::fstream	fileStream(fileToOpen.c_str());
	if (fileStream) {
		_repsonseBody.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
		fileStream.close();
		buildHeader();
	} else {
		setStatus("500 Internal Server Error"); // Failed to open the file
	}
	_done = true;
}

bool	Request::validateRequest(const std::string& method) {
	size_t	firstSlashPos = _path.find('/');
	size_t	secondSlashPos = _path.find('/', firstSlashPos + 1);
	const std::string requestedLocation;
	if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos)
		requestedLocation = _path.substr(firstSlashPos, secondSlashPos - firstSlashPos);
	else
		requestedLocation = _path;
	_location = _config.getLocation(requestedLocation);
	if (_location.getLocation() == "null") {
		setStatus("404 Not Found");
		return false;
	}
	else if (!_location.isAcceptedMethod(method)) {
		setStatus("403 Forbidden");
		return false;
	}
	else
		return true;
}

void	Request::generateAutoIndex(std::string &uri) {

	DIR *dir = opendir(uri.c_str());

	if (!dir) {
		std::cerr << "Error opening directory" << std::endl;
		return;
	}
	_repsonseBody += "<html>\n <head>\n <title>Index of " + uri + "</title>\n </head>\n <body>";
	_repsonseBody += "<h1>Index of " + uri + "</h1>";
	_repsonseBody += "<ul>\n";

	struct dirnet	currDir;
	
	buildHeader()

}

void	Request::handleGetMethod(std::string &fileToOpen){
	
	if (!validateRequest("GET") || access(fileToOpen.c_str(), R_OK) == -1)
		return;
	if (_path.back() == '/') { // path is a directory
		if (!_location.getIndex.empty() && _location.isIndexFile(_path)) {
			fileToOpen += _location.getIndex();
			buildResponse(fileToOpen);
		}
		else if (_location.getDirectoryListing())
			generateAutoIndex(fileToOpen);

		else
			setStatus("404 Not Found");
		
	}
	else { // path is a file
		
	}
	_done = true;
}

void	Request::handlePostMethod(){
	if (!_location.isAcceptedMethod("POST"))
		setStatus("403 Forbiden");
}

void	Request::handleDeleteMethod(std::string &fileToOpen){
	if (!_location.isAcceptedMethod("DELETE"))
		setStatus("403 Forbiden");
}

std::string	Request::extractPathFromUrl(std::string& url) {
	size_t	firstSlashPos = url.find('/');
	size_t	secondSlashPos = url.find('/', firstSlashPos + 1);

	if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos) {
		size_t	pathStartPos = secondSlashPos;
		std::string	path = url.substr(pathStartPos);
		if (path[1])
			return getRoot() + path;
	}
	return url;
}

void	Request::handleRequest(int clientSocket) {
	std::string	fileToOpen;

	fileToOpen = extractPathFromUrl(_path);
	if (access(fileToOpen.c_str(), F_OK) == -1)
		setStatus("404 Not Found");
	parseHeader(clientSocket);
	if (_method == "GET")
		handleGetMethod(fileToOpen);
	else if (_method == "POST")
		handlePostMethod();
	else if (_method == "DELETE")
		handleDeleteMethod(fileToOpen);
	else
		setStatus("405 Method Not Allowed");
	if (!_done)
		buildResponse();
}

void Request::printData()
{
	std::cout << "Request _method:    " << _method << std::endl;
	std::cout << "Request _path:    " << _path << std::endl;
	std::cout << "Request _version:    " << _version << std::endl;
	std::cout << "Request _host:    " << _host << std::endl;
	std::cout << "Request _contentType:    " << _contentType << std::endl;
	std::cout << "Request _contentLength:    " << _contentLength << std::endl;
	//std::cout << "Request _body:    " << _body << std::endl;

}

std::string Request::getPath() const
{
	return _path;
}

void	Request::setStatus(const std::string &status) {
	_status = status;
}

std::string	Request::getResponseHeader() const {
	return _repsonseHeader;
}

std::string	Request::getResponseBody() const {
	return _repsonseBody;
}

/* std::string Request::getHeader() const
{
	return _header;
}

std::string Request::getMethod() const
{
	return _method;
}


std::string Request::getExtension() const
{
	return _extension;
} */