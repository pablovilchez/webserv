#include "WebServer.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include "Config.hpp"


int main() {
/*     Config conf;
    conf.printData(); */
    //WebServer server("config.conf");
    Server server;
    server.startServer();

    return 0;
}
