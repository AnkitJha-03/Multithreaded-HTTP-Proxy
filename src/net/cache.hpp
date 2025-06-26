#pragma once
#include <list>
#include <string>
#include <unordered_map>

class LRUCache {
 public:
  LRUCache(int capacity);
  std::string get(const std::string& key);
  void put(const std::string& key, const std::string& value);
  bool contains(const std::string& key) const;

 private:
  int capacity;
  std::list<std::string> access_order;
  // key -> {value, iterator to access_order}
  std::unordered_map<std::string,
                     std::pair<std::string, std::list<std::string>::iterator>>
      cache_map;

  void moveToFront(const std::string& key);
  void evict();
};