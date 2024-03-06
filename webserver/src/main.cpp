#include "WebServer.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include "Config.hpp"

#define PORT 8080
#define MAX_CONNECTIONS 3
#define MAX_BUFFER_SIZE 1024

int main() {
/*     Config conf;
    conf.printData(); */
    //WebServer server("config.conf");
    Server server;
    server.startServer();

    return 0;
}
