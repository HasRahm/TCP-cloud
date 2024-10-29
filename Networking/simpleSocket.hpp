#ifndef SIMPLESOCKET_HPP
#define SIMPLESOCKET_HPP

#include <winsock2.h> // Use Winsock 2
#include <string>

namespace http {
    class TcpServer {
    public:
        // Constructor with parameters
        TcpServer(std::string ip_address, int port);
        ~TcpServer();

        // Public methods for controlling the server
        void startListen();
        

    private:
        std::string m_ip_address;
        int m_port;
        SOCKET m_socket = INVALID_SOCKET;
        SOCKET m_new_socket = INVALID_SOCKET;
        int m_incomingMessage = 0;
        struct sockaddr_in m_socketAddress;
        int m_socketAddress_len;
        std::string m_serverMessage;
        WSADATA m_wsaData;

        // Private methods
        int startServer();
        void closeServer();
        std::string buildResponse();
        void acceptConnection(SOCKET &new_socket);
        void sendResponse();
    };
}

#endif // SIMPLESOCKET_HPP
