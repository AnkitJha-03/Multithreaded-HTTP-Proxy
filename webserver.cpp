#include <winsock2.h>
#include <iostream>
#include <thread>
#include <atomic>

const int PORT = 8080;
const int WINSOCK_VERS = MAKEWORD(2,2);
const int BUFFER_SIZE = 1024;
const int PENDING_CONNECTIONS = 10;

std::atomic<int> connection_counter(0);

// Function to handle client requests
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
            response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "\r\n"
                       "Server is running";
        } else {
            // Extracting the requested URL from the path
            const char* path_start = strstr(request, "GET /") + 5;
            const char* path_end = strchr(path_start, ' ');
            
            if (path_start == nullptr || path_end == nullptr) {
                response = "HTTP/1.1 400 Bad Request\r\n"
                           "\r\n";
            } else {
                std::string url(path_start, path_end - path_start);
                
                response = "HTTP/1.1 301 Moved Permanently\r\n"
                           "Location: http://" + url + "\r\n"
                           "\r\n";
            }
        }

        // connection connection_counters with a delay
        connection_counter++;
        std::cout << "Thread " << std::this_thread::get_id() << " handling connection #" << connection_counter << std::endl;
        Sleep(10000);

        // Send the response back to the client
        if (send(client_socket, response.c_str(), response.size(), 0) == SOCKET_ERROR) {
            throw std::runtime_error("Error sending response to client");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in HandleClient: " << e.what() << std::endl;
    }
    
    // Close the client connection
    closesocket(client_socket);
}

int main() {
    SOCKET server_socket = INVALID_SOCKET;

    try {
        // Initializing Winsock
        WSADATA wsa_data;

        if (WSAStartup(WINSOCK_VERS, &wsa_data) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }

        // Creating a TCP socket
        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create socket");
        }

        // Server information
        sockaddr_in server_info;
        server_info.sin_family = AF_INET;
        server_info.sin_port = htons(PORT);
        server_info.sin_addr.s_addr = INADDR_ANY;

        // Binding the socket to the server info
        if ((bind(server_socket, (sockaddr*)&server_info, sizeof(server_info))) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to bind to port " + std::to_string(PORT));
        }

        // Start listening for incoming connections
        if (listen(server_socket, PENDING_CONNECTIONS) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to listen on socket");
        }

        std::cout << "Server running on http://localhost:" << PORT << std::endl;
        std::cout << "Press Ctrl+C to stop the server..." << std::endl;

        // Main server loop
        while (true) {
            // Accepting a new client connection
            SOCKET client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket == INVALID_SOCKET) {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }

            // launching a new thread to handle the client request
            std::thread(HandleClient, client_socket).detach();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    // Cleanup
    std::cout << "Shutting down server..." << std::endl;
    if (server_socket != INVALID_SOCKET) {
        closesocket(server_socket);
    }
    WSACleanup();
    return 0;
}