#include "WebServer.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include "Config.hpp"


int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    WebServer webserver(argv[1]);
    return 0;
}
