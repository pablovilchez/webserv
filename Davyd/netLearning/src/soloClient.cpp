#include "header.hpp"

void soloClient() {
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];
	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int clientSocket = accept(listening, reinterpret_cast<sockaddr*>(&client), &clientSize);

	if (clientSocket == -1) {
		std::cerr << "Problem with client connection" << std::endl;
		exit (-4);
	}

	// Close the listening socket
	close(listening);

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
		/*
			if the last arg will be 0 recv() == read(clientSocket, buf, 4096);
		*/
		int bytesRecv = recv(clientSocket, buf, 4096, 0); // receive a message from a socket

		if (bytesRecv == -1) {
			std::cerr << "There was a connection issue" << std::endl;
			break;
		}

		if (bytesRecv == 0) {
			std::cout << "The client desconnected" << std::endl;
			break;
		}

		std::string receivedData(buf, bytesRecv);
		receivedData.erase(receivedData.find_last_not_of(" \n\r\t") + 1);
		receivedData.erase(0, receivedData.find_first_not_of(" \n\r\t"));
		if (receivedData == "exit") {
			std::cout << "Shutting down..." << std::endl;
			break ;
		}
		else {
			// Echo received data back to the client
			// Resend message
			/*
				if the last arg will be 0, socket() is == write(clientSocket, buf, bytesRecv + 1);
			*/
			if (send(clientSocket, buf, bytesRecv + 1, 0) == -1) {
				perror("Error sending message");
				break;
			}


			// Echo message
			std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;
		}
	}
	// Close socket
	close(clientSocket);
}
