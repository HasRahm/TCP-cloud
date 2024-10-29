#include "simpleSocket.hpp"
#include <iostream>
#include <sstream>
#include <winsock2.h> // Use Winsock 2 for Windows

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

namespace {
    const int BUFFER_SIZE = 30720;

    void log(const std::string &message) {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string &errorMessage) {
        std::cerr << "Error Code: " << WSAGetLastError() << std::endl;
        log("ERROR: " + errorMessage);
        exit(1);
    }
}

namespace http {
    TcpServer::TcpServer(std::string ip_address, int port)
        : m_ip_address(ip_address), m_port(port), m_socket(), m_new_socket(),
          m_incomingMessage(), m_socketAddress_len(sizeof(m_socketAddress)), m_serverMessage(buildResponse()),m_wsaData() {

        // Initialize socket address before starting the server
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(port);
        m_socketAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());

        if (startServer() != 0) {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
            log(ss.str());
        }
    }

    TcpServer::~TcpServer() {
        closeServer();
    }

    int TcpServer::startServer() {
        if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
            exitWithError("WSAStartup failed!");
        }

        // Create a TCP socket
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) {  // Use INVALID_SOCKET for error checking in Winsock
            exitWithError("Cannot create socket");
            return 1;
        }

        // Bind the socket
        if (bind(m_socket, (struct sockaddr *)&m_socketAddress, m_socketAddress_len) == SOCKET_ERROR) {
            exitWithError("Cannot bind to the server");
            return 1;
        }

        return 0;
    }

    void TcpServer::closeServer() {
        closesocket(m_socket);
        closesocket(m_new_socket);
        WSACleanup();
        log("Server closed.");
        exit(0);
    }

    void TcpServer::startListen() {
        if (listen(m_socket, 20) == SOCKET_ERROR) {
            exitWithError("Listen failed");
        }

        std::ostringstream ss;
        ss << "\n*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr)
           << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
        log(ss.str());

        char buffer[BUFFER_SIZE] = {0};
        int bytesReceived = recv(m_new_socket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == SOCKET_ERROR) {
            exitWithError("Failed to receive bytes from client socket connection");
        }
    }

    void TcpServer::acceptConnection(SOCKET &new_socket) {
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
        if (new_socket == INVALID_SOCKET) {
            std::ostringstream ss;
            ss << "Server failed to accept incoming connection from Address: "
               << inet_ntoa(m_socketAddress.sin_addr) << " PORT: " << ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());
        }
    }

    void TcpServer::sendResponse() {
        int bytesSent;
        long totalBytesSent = 0;
        while (totalBytesSent < m_serverMessage.size()) {
            bytesSent = send(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size(), 0);
            if (bytesSent == SOCKET_ERROR) {
                break;
            }
            totalBytesSent += bytesSent;
        }

        if (totalBytesSent == m_serverMessage.size()) {
            log("---- Server Response sent to Client ----\n\n");
        } else {
            log("---- Server Response failed to send to Client ----\n\n");
        }
    }
}
