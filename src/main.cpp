#include "core/server.h"
#include "core/thread_pool.h"

int main() {
    int PORT = 8080;
    int PENDING_CONNECTIONS = 5;
    ThreadPool pool(2);
    Server server(PORT, PENDING_CONNECTIONS, pool);
    server.start();
    server.stop();
    return 0;
}
