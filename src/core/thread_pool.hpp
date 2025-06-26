#pragma once
#include <winsock2.h>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(int size);
  void enqueue(SOCKET client_socket);
  void end();

 private:
  std::vector<std::thread> workers;
  std::queue<SOCKET> client_queue;
  std::mutex queue_mutex;
  std::condition_variable cv;
  bool stop;

  void workerThread();
};
