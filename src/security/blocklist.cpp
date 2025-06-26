#include "blocklist.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

Blocklist::Blocklist() {
  std::ifstream file("C:/Projects/Web Server Proxy/src/data/blocklist.txt");
  std::string line;

  while (std::getline(file, line)) {
    // Remove whitespace
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

    // Skip comments and empty lines
    if (line.empty() || line[0] == '#') continue;

    if (!line.empty()) {
      banned_domains.push_back(line);
    }
  }
}

bool Blocklist::is_blocked(const std::string& domain) const {
  for (const auto& banned : banned_domains) {
    // Check if banned domain exists anywhere in the requested domain
    if (domain.find(banned) != std::string::npos) {
      return true;
    }
  }
  return false;
}