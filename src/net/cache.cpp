#include "cache.hpp"

#include <stdexcept>

LRUCache::LRUCache(int capacity) : capacity(capacity) {
  if (capacity == 0) {
    throw std::invalid_argument("Cache capacity must be greater than 0");
  }
}

bool LRUCache::contains(const std::string& key) const {
  return cache_map.find(key) != cache_map.end();
}

std::string LRUCache::get(const std::string& key) {
  auto it = cache_map.find(key);
  if (it == cache_map.end()) {
    return "";
  }

  // Move the key to front of access order
  moveToFront(key);
  return it->second.first;
}

void LRUCache::put(const std::string& key, const std::string& value) {
  if (cache_map.find(key) != cache_map.end()) {
    // Update existing value
    cache_map[key].first = value;
    moveToFront(key);
    return;
  }

  // Check if we need to evict
  if (cache_map.size() >= capacity) {
    evict();
  }

  // Add new entry
  access_order.push_front(key);
  cache_map[key] = {value, access_order.begin()};
}

void LRUCache::moveToFront(const std::string& key) {
  auto it = cache_map.find(key);
  if (it == cache_map.end()) {
    return;
  }

  // Move to front of access order
  access_order.erase(it->second.second);
  access_order.push_front(key);
  it->second.second = access_order.begin();
}

void LRUCache::evict() {
  if (access_order.empty()) {
    return;
  }

  // Remove least recently used item
  const std::string& lru_key = access_order.back();
  cache_map.erase(lru_key);
  access_order.pop_back();
}