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
	/*
		SOCK_STREAM - TCP(reliable, connection oriented) (send(), recv())
		SOCK_DGRAM - UDP(unreliable/faster, connectionless) (sendto(), recvfrom())
	*/
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
	/*
		struct sockaddr_in {
			short int          sin_family;  // Address family
			unsigned short int sin_port;    // Port number
			struct in_addr     sin_addr;    // Internet address
			unsigned char      sin_zero[8]; // Same size as struct sockaddr
		};

		struct in_addr {
    		unsigned long s_addr; // that's a 32-bit long, or 4 bytes
		};
	*/
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

	char hostName[1024];
    gethostname(hostName, 1024);
    std::cout << "Server started on " << hostName << std::endl;

	hostent* host = gethostbyname(hostName);
	if (host == NULL) {
		herror("gethostbyname");
		exit(1);
	}
	/*
	inet_ntoa(), inet_aton()
	These functions convert between the string form of an IP address and its binary representation.
	n - network
	a - ascii
	Function inet_addr() is an older function that does basically the same thing as inet_aton(). It's theoretically deprecated, but you'll see it alot.
	*/
	std::cout << "IP Address: " << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;

	/*
		struct hostent {
			char *h_name;        official name of host
			char **h_aliases;    A list of aliases that can be accessed with arrays--the last element is NULL
			int h_addrtype;      he result's address type, which really should be AF_INET for our purposes.
			int h_length;        length of address in bytes, whitch is 4 for IPv4 adresses
			char **h_addr_list;  List of IP addresses. Although this is a char**, it's really an array of struct in_addr*s in disguise. The last array element is NULL.
			h_addr;             commonly defined alias for h_addr_list[0]
		};
	*/

	//Accept a call
	multyClient();
	return 0;
}

