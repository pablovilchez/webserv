#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

/*
	USAGE:

	run the porgram - will be a server
	open other terminal and execute this "telnet localhost 54000 - will be a client"
*/

/*The plan:
1 - Create Socket
2 - Bind the socket to a IP /port
3 - Mark the socket for listening in
4 - Accept a call
5 - Close the listening socket
6 - While recieving - deplay messag, echo message
7 - Close socket
*/

int main() {
	// Create Socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	// std::cout << listening -> 3 - like open();
	if (listening == -1) {
		std::cerr << "Can't create a socket" << std::endl;
		return -1;
	}

	// Bind the socket to a IP /port
    sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // little endian - big endian
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // 127.0.0.1 return ""

	// Cast the sockaddr_in pointer to sockaddr pointer
	if(bind(listening, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1) {
		std::cerr << "Can't bind to IP/port" << std::endl;
		return -2;
	}

	//Mark the socket for listening in
	if (listen(listening, SOMAXCONN) == -1) {
		std::cerr << "Can't listen" << std::endl;
		return -3;
	}

	//Accept a call
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept(listening, reinterpret_cast<sockaddr*>(&client), &clientSize); // Read about accept() function - it works like signal that hendel every client request

	if (clientSocket == -1) {
		std::cerr << "Problem with client connection" << std::endl;
		return -4;
	}

	// Close the listening socket
	//close(listening);

	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo(reinterpret_cast<sockaddr*>(&client), sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

	if (result) {
		std::cout << host << " connected on " << svc << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
	}

	//While recieving - deplay messag, echo message
	char buf[4096];
	while (true) {
		// Clear buffer
		memset(buf, 0, 4096);
		// Wait for a message
		int bytesRecv = recv(clientSocket, buf, 4096, 0);
		if (bytesRecv == -1) {
			std::cerr << "There was a connetcion issue" << std::endl;
			break;
		}

		if (bytesRecv == 0) {
			std::cout << "The client desconnected" << std::endl;
			break;
		}

		// Display message
		std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

		// Resend message
		send(clientSocket, buf, bytesRecv + 1, 0);
	}
	// Close socket
	close(clientSocket);
	return 0;
}

