#include "thread_pool.hpp"
#include <winsock2.h>

class Server {
 public:
  Server(int port, int connections, ThreadPool& threadPool);
  void start();
  void stop();

 private:
  int port;
  SOCKET server_socket;
  ThreadPool& threadPool;
  int PENDING_CONNECTIONS;

  void initializeWinsock();
  void createSocket();
  void bindSocket();
  void startListening();
  void acceptConnections();
};
