#include "logger.hpp"
#include "server.hpp"
#include <iostream>
#include <stdexcept>

Server::Server(int port, int connections, ThreadPool &threadPool)
    : port(port), PENDING_CONNECTIONS(connections), threadPool(threadPool), server_socket(INVALID_SOCKET) {
}

void Server::initializeWinsock() {
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
    throw std::runtime_error("Failed to initialize Winsock");
  }
}

void Server::createSocket() {
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket == INVALID_SOCKET) {
    throw std::runtime_error("Failed to create socket");
  }
}

void Server::bindSocket() {
  sockaddr_in server_info;
  server_info.sin_family = AF_INET;
  server_info.sin_port = htons(port);
  server_info.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (sockaddr *)&server_info, sizeof(server_info)) ==
      SOCKET_ERROR) {
    throw std::runtime_error("Failed to bind to port " + std::to_string(port));
  }
}

void Server::startListening() {
  if (listen(server_socket, PENDING_CONNECTIONS) == SOCKET_ERROR) {
    throw std::runtime_error("Failed to listen on socket");
  }
  Logger::info("Server started on port " + std::to_string(port));
}

void Server::acceptConnections() {
  while (true) {
    SOCKET client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket != INVALID_SOCKET) {
      threadPool.enqueue(client_socket);
    }
  }
}

void Server::start() {
  try {
    initializeWinsock();
    createSocket();
    bindSocket();
    startListening();
    acceptConnections();
  } catch (const std::exception &e) {
    Logger::error(std::string("Exception in Server: ") + e.what());
    std::cerr << "Error in Server: " << e.what() << std::endl;
    stop();
  }
}

void Server::stop() {
  Logger::info("Shutting down server...");

  // stop the thread pool
  threadPool.end();

  // close the server socket
  if (server_socket != INVALID_SOCKET) {
    closesocket(server_socket);
    server_socket = INVALID_SOCKET;
  }
  // cleanup Winsock
  WSACleanup();
}