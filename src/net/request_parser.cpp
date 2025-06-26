#include "request_parser.hpp"
#include "cache.hpp"
#include <cstring>
#include <mutex>

// Global cache
LRUCache response_cache(100);
std::mutex cache_mutex;

std::string request_parser(const char request[]) {
  // Check if the request is for the root path ("/")
  bool is_root_path = (strstr(request, "GET / HTTP") != nullptr);

  if (is_root_path) {
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "\r\n"
           "Server is running";
  }

  // Extracting the requested URL from the path
  const char* path_start = strstr(request, "GET /") + 5;
  const char* path_end = strchr(path_start, ' ');

  if (path_start == nullptr || path_end == nullptr) {
    return "HTTP/1.1 400 Bad Request\r\n"
           "\r\n";
  }

  std::string url(path_start, path_end - path_start);

  // Normalize URL by removing http:// or https:// prefix
  if (url.rfind("http://", 0) == 0) {
    url = url.substr(7);  // remove "http://"
  } else if (url.rfind("https://", 0) == 0) {
    url = url.substr(8);  // remove "https://"
  }
  // Remove trailing slash
  if (!url.empty() && url.back() == '/') {
    url.pop_back();
  }

  // Check cache first
  std::string cache_key = "GET_" + url;
  {
    std::lock_guard<std::mutex> lock(cache_mutex);
    if (response_cache.contains(cache_key)) {
      std::cout << "Cache hit for key: " << cache_key << std::endl;
      return response_cache.get(cache_key);
    }
  }

  // Generate response if not in cache
  std::string response =
      "HTTP/1.1 301 Moved Permanently\r\n"
      "Location: http://" +
      url +
      "\r\n"
      "\r\n";

  // Store in cache
  {
    std::lock_guard<std::mutex> lock(cache_mutex);
    response_cache.put(cache_key, response);
  }

  return response;
}