#include "logger.hpp"
#include "request_parser.hpp"
#include "blocklist.hpp"
#include "cache.hpp"
#include <algorithm>
#include <cstring>
#include <mutex>

// Global cache
LRUCache response_cache(100);
std::mutex cache_mutex;

// Global Blocklist
Blocklist blocklist;

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
  if (url.find("http://", 0) == 0) {
    url = url.substr(7);  // remove "http://"
  } else if (url.find("https://", 0) == 0) {
    url = url.substr(8);  // remove "https://"
  }
  // Remove trailing slash
  if (!url.empty() && url.back() == '/') {
    url.pop_back();
  }
  // Convert URL to lowercase for case-insensitive comparison
  std::transform(url.begin(), url.end(), url.begin(), ::tolower);

  // Check if the URL is blocked
  if (blocklist.is_blocked(url)) {
    return "HTTP/1.1 403 Forbidden\r\n"
           "\r\n";
  }

  // Check cache first
  std::string cache_key = "GET_" + url;
  {
    std::lock_guard<std::mutex> lock(cache_mutex);
    if (response_cache.contains(cache_key)) {
      Logger::info("Cache hit for URL: " + url);
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