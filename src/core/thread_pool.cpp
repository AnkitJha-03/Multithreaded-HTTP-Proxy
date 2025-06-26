#include "thread_pool.hpp"
#include "client_handler.hpp"

ThreadPool::ThreadPool(int size) : stop(false) {
  for (int i = 0; i < size; i++) {
    workers.emplace_back([this] { workerThread(); });
  }
}

void ThreadPool::end() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  cv.notify_all();

  for (auto& worker : workers) {
    if (worker.joinable()) worker.join();
  }
}

void ThreadPool::enqueue(SOCKET client_socket) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    client_queue.push(client_socket);
  }
  cv.notify_one();
}

void ThreadPool::workerThread() {
  while (true) {
    SOCKET client_socket;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      cv.wait(lock, [this] { return !client_queue.empty() || stop; });

      if (stop && client_queue.empty()) return;

      client_socket = client_queue.front();
      client_queue.pop();
    }
    HandleClient(client_socket);
  }
}