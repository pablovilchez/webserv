
#include "header.hpp"
#include "Response.hpp"
#define MAX_BUFFER_SIZE 1024
#define PORT 8080

int main() {
	// Create Socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		std::cerr << "Can't create a socket" << std::endl;
		return -1;
	}

	// Bind the socket to a IP /port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	hint.sin_addr.s_addr = INADDR_ANY;
	// the binary form of the IP address will be stored
	if (inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr) == -1) {
		std::cerr << "inet_pton failed" << std::endl;
		return -1;
	}
	memset(&(hint.sin_zero), '\0', 8); // Zero the rest of the struct 

	// Set socket option to reuse the address
	int opt = 1;
	if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "setsockopt failed" << std::endl;
		return -2;
	}
 
	// Cast the sockaddr_in pointer to sockaddr pointer
	if(bind(listening, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1) {
		std::cerr << "Can't bind to IP/port" << std::endl;
		return -2;
	}

	// Mark the socket for listening in
	if (listen(listening, SOMAXCONN) == -1) {
		std::cerr << "Can't listen" << std::endl;
		return -3;
	}

	// Accept a call
	int resp = multyClient(listening);
	return resp;
}