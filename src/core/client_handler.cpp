#include "client_handler.h"
#include <atomic>
#include <iostream>
#include <thread>

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

    // Check if the request is for the root path ("/")
    bool is_root_path = (strstr(request, "GET / HTTP") != nullptr);

    std::string response;

    if (is_root_path) {
      response =
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n"
          "\r\n"
          "Server is running";
    } else {
      // Extracting the requested URL from the path
      const char* path_start = strstr(request, "GET /") + 5;
      const char* path_end = strchr(path_start, ' ');

      if (path_start == nullptr || path_end == nullptr) {
        response =
            "HTTP/1.1 400 Bad Request\r\n"
            "\r\n";
      } else {
        std::string url(path_start, path_end - path_start);

        response =
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: http://" +
            url +
            "\r\n"
            "\r\n";
      }
    }

    // connection connection_counters with a delay
    connection_counter++;
    std::cout << "Thread " << std::this_thread::get_id()
              << " handling connection #" << connection_counter << std::endl;
    Sleep(10000);

    // Send the response back to the client
    if (send(client_socket, response.c_str(), response.size(), 0) ==
        SOCKET_ERROR) {
      throw std::runtime_error("Error sending response to client");
    }
  } catch (const std::exception& e) {
    std::cerr << "Error in HandleClient: " << e.what() << std::endl;
  }

  // Close the client connection
  closesocket(client_socket);
}