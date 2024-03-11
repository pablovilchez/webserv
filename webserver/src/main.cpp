#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>

#include "WebServer.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include "Config.hpp"


int main(int argc, char **argv) {
    
    WebServer server(argv[1]);

    return 0;
}
