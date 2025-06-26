#include "logger.hpp"
#include "client_handler.hpp"
#include "request_parser.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <sstream>

const int BUFFER_SIZE = 1024;
std::atomic<int> connection_counter(0);

void HandleClient(SOCKET client_socket) {
  try {
    char request[BUFFER_SIZE] = {0};

    // Receive the request from the client
    int bytes_received = recv(client_socket, request, sizeof(request), 0);
    if (bytes_received == SOCKET_ERROR) {
      throw std::runtime_error("Error receiving data from client");
    }

    // Parse the request (will use cache internally)
    std::string response = request_parser(request);

    // connection counters with a delay
    connection_counter++;
    std::ostringstream oss;
    oss << "Thread " << std::this_thread::get_id()
        << " handling connection #" << connection_counter;
    Logger::info(oss.str());
    Sleep(10000);

    // Send the response back to the client
    if (send(client_socket, response.c_str(), response.size(), 0) == SOCKET_ERROR) {
      throw std::runtime_error("Error sending response to client");
    }
  } catch (const std::exception& e) {
    Logger::error(std::string("Exception in HandleClient: ") + e.what());
    std::cerr << "Error in HandleClient: " << e.what() << std::endl;
  }

  // Close the client connection
  closesocket(client_socket);
}