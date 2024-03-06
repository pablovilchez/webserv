#include "header.hpp"
#include "Request.hpp"

std::string readFile(const char *filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open file: " << filename << std::endl;
		return "";
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}


int multyClient(int listening) {
fd_set master; // Master file descriptor list
FD_ZERO(&master); // Clear the master set
FD_SET(listening, &master); // Add the listening socket to the master set

int maxFd = listening; // Track the maximum file descriptor

bool runningFlag = true;

while (runningFlag) {
	fd_set copy = master; // Copy the master set

	int socketCount = select(maxFd + 1, &copy, NULL, NULL, NULL);

	if (socketCount == -1) {
		std::cerr << "Select error" << std::endl;
		return -4;
	}

	for (int i = 0; i <= maxFd; ++i) {
		if (FD_ISSET(i, &copy)) { // Check if the file descriptor is in the set
			if (i == listening) { // New connection
				// Accept a call
				sockaddr_in client;
				socklen_t clientSize = sizeof(client);
				int clientSocket = accept(listening, reinterpret_cast<sockaddr*>(&client), &clientSize);

				if (clientSocket == -1) {
					std::cerr << "Problem with client connection" << std::endl;
					continue;
				}

				FD_SET(clientSocket, &master); // Add the new client socket to the master set
				if (clientSocket > maxFd) {
					maxFd = clientSocket; // Update the maximum file descriptor
				}

				// Print client information
				char host[NI_MAXHOST];
				char svc[NI_MAXSERV];
				memset(host, 0, NI_MAXHOST);
				memset(svc, 0, NI_MAXSERV);

				int result = getnameinfo(reinterpret_cast<sockaddr*>(&client), clientSize, host, NI_MAXHOST, svc, NI_MAXSERV, 0);

				if (result) {
					std::cout << host << " connected on " << svc << std::endl;
				} else {
					inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
					std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
				}
			} else { // Existing client activity
				char buf[4096];
				memset(buf, 0, 4096);

				// Receive data from the client
				int bytesRecv = recv(i, buf, sizeof(buf), 0);
				if (bytesRecv <= 0) {
					// Error or connection closed
					if (bytesRecv == 0) {
						std::cout << "Client " << i << " disconnected" << std::endl;
					} else {
						std::cerr << "Recv error on client " << i << std::endl;
					}
					close(i); // Close the socket
					FD_CLR(i, &master); // Remove the socket from the master set

				} else {
					std::string	receivedData(buf, bytesRecv);
					std::string	response;
					Request	receivedData2(buf, i);
					//receivedData2.printData();
					receivedData.erase(receivedData.find_last_not_of(" \n\r\t") + 1);
					receivedData.erase(0, receivedData.find_first_not_of(" \n\r\t"));
					if (receivedData == "exit") {
						std::cout << "Client " << i << " requested disconnection" << std::endl;
						close(i); // Close the client's socket
						FD_CLR(i, &master);
					}
					else if (receivedData.find("GET") == 0) {
	/* 					if (receivedData.find("GET / HTTP/1.1") != std::string::npos)
							std::cout << "Received HTTP request for main page:\n" << receivedData << std::endl; */
						std::string htmlContent = readFile("ex2.html");
						std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + htmlContent;
						send(i, response.c_str(), response.size(), 0);
						// Close the client's socket
						std::cout << "Client " << i << " served and disconnected" << std::endl;
						close(i);
						FD_CLR(i, &master);
					}
					else if (receivedData.find("POST") == 0) {
					//	receivedData2.printData();
						
						std::string bodyDelimiter = "\r\n\r\n";
						size_t bodyStart = receivedData.find(bodyDelimiter);

						if (bodyStart != std::string::npos) {
							// Extract the POST data from the request body
							std::string postData = receivedData.substr(bodyStart + bodyDelimiter.length());
							// Now you have the POST data, you can further parse it based on your needs
							//std::cout << "Received POST data:\n" << postData << std::endl;
/* 							std::ofstream outputFile("uploaded_image.jpg", std::ios::binary);
							outputFile << postData;
							outputFile.close(); */

							//std::cout << "File uploaded successfully." << std::endl;
					}
					}
					else {
						// Echo received data back to the client
						//std::cout << "Received: " << receivedData << std::endl;
						send(i, response.c_str(), response.size(), 0);
					}
				}
			}
		}
	}
}
	for (int i = 0; i <= maxFd; ++i) {
		if (FD_ISSET(i, &master))
			close(i);
	}

	// Close listening socket
	close(listening);
	return 0;
}

