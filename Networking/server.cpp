#include "simpleSocket.hpp"

int main(){
    using namespace http;
    
    // Create the server
    TcpServer server("0.0.0.0", 8080);

    // Start listening for incoming connections
    server.startListen();

    return 0;
}
