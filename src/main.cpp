#include "logger.hpp"
#include "server.hpp"
#include "thread_pool.hpp"

int main() {
    // Initialize the logger
    Logger::init("C:/Projects/Web Server Proxy/src/data/proxy.log");

    // Start the server
    const int PORT = 8080;
    const int PENDING_CONNECTIONS = 5;
    ThreadPool pool(2);
    Server server(PORT, PENDING_CONNECTIONS, pool);
    server.start();
    server.stop();
    return 0;
}