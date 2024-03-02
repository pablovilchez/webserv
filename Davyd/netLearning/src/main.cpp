#include "header.hpp"

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

#define DEST_PORT 54000
#define DEST_IP "127.0.0.1"

int listening = 0;

int main() {
	// Create Socket
	listening = socket(PF_INET, SOCK_STREAM, 0);
	char yes = 1;
	if (listening == -1) {
		perror("Can't create a socket");
		exit (1);
	}

	if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	exit(1);
	}
	if (setsockopt(listening, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	exit(1);
	}

	// Bind the socket to a IP /port
    sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(DEST_PORT);
	hint.sin_addr.s_addr = INADDR_ANY;
	memset(&(hint.sin_zero), '\0', 8);

	// Cast the sockaddr_in pointer to sockaddr pointer
	if(bind(listening, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1) {
		perror("Can't bind to IP/port");
		exit(1);
	}

	//Mark the socket for listening in
	if (listen(listening, SOMAXCONN) == -1) {
		perror("Can't listen");
		exit (1);
	}

	//Accept a call
	multyClient();
	return 0;
}

