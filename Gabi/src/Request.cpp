/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:18:22 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/06 11:55:47 by gkrusta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &raw, int clientSocket) : _raw(raw)
{
	_method = "";
	_path = "";
	_version = "";
	_host = "";
	_contentType = "";
	_contentLength = 0;
	_body = "";
	_number = 0;
	parseContent(clientSocket);
}

Request::~Request() { }

std::string Request::getRaw() const
{
	return _raw;
}
#include <vector>

void Request::parseBody(int clientSocket) {
	// Check if Content-Length header is present
	if (_contentLength == 0) {
		std::cerr << "Content-Length header is missing or zero." << std::endl;
		return;
	}
	// Check if the request has "Expect: 100-continue"
	size_t expectHeaderPos = _raw.find("Expect: 100-continue");
	if (expectHeaderPos != std::string::npos) {

		bool readyToAcceptPayload = true;

		if (readyToAcceptPayload) {
			std::string continueResponse = "HTTP/1.1 100 Continue\r\n\r\n";
			send(clientSocket, continueResponse.c_str(), continueResponse.size(), 0);
		} else {
			// Send an immediate final response if not ready
			std::string finalResponse = "HTTP/1.1 400 Bad Request\r\n\r\n";
			send(clientSocket, finalResponse.c_str(), finalResponse.size(), 0);
			// Close the connection or handle accordingly
		}
	}
	// Find the start of the body
	std::string boundary = "\r\n\r\n";
	size_t bodyStart = _raw.find(boundary) + boundary.size();

	// Create a vector with the size of the content length
	std::vector<char> bodyData(_contentLength);

	// Copy the binary data from the body into the vector
	std::copy_n(_raw.begin() + bodyStart, _contentLength, bodyData.begin());

	// Identify content type
	std::string fileExtension;
	if (_contentType.find("image/jpeg") != std::string::npos) {
		fileExtension = ".jpg";
	} else if (_contentType.find("image/png") != std::string::npos) {
		fileExtension = ".png";
	} else {
		std::cerr << "Unsupported content type." << std::endl;
		return;
	}

	// Save the file with the appropriate extension
	std::ofstream outputFile("uploaded_image" + fileExtension, std::ios::binary);
	if (outputFile.is_open()) {
		// Write the binary data to the file
		outputFile.write(bodyData.data(), bodyData.size());
		outputFile.close();
		std::cout << "File saved: uploaded_image" << fileExtension << std::endl;
	} else {
		std::cerr << "Error opening file for writing." << std::endl;
	}
}

void Request::parseContent(int clientSocket)
{
	size_t	pos = 0;
	size_t	end = _raw.find("\r\n");
	std::cout << "Raw Request:\n" << _raw << std::endl;
	// parse request line
	if (end != std::string::npos) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	space1 = line.find(' ');
		size_t	space2 = line.find(' ', space1 + 1);

		if (space1 != std::string::npos || space2 != std::string::npos) {
			_method = line.substr(0, space1);
			_path = line.substr(space1 + 1, space2 - space1 - 1);
			_version = line.substr(space2 + 1);
		}
		pos = end + 2; // Move to the next position after "\r\n"
	}

	// parse header
	while ((end = _raw.find("\r\n", pos)) != std::string::npos && pos < _raw.size()) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	colPos = line.find(':');
		if (colPos != std::string::npos) {
			std::string	key = line.substr(0, colPos);
			std::string	value = line.substr(colPos + 2);
			if (key == "Host")
				_host = value;
			else if (key == "Content-Type")
				_contentType = value;
			else if (key == "Content-Length")
				_contentLength = atoi(value.c_str());
		}
		pos = end + 2;
	}
	if (_method == "POST")
		parseBody(clientSocket);
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

/* std::string Request::getHeader() const
{
	return _header;
}

std::string Request::getMethod() const
{
	return _method;
}

std::string Request::getPath() const
{
	return _path;
}

std::string Request::getExtension() const
{
	return _extension;
} */