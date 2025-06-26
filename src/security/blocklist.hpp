#pragma once
#include <string>
#include <vector>

class Blocklist {
 private:
  std::vector<std::string> banned_domains;

 public:
  Blocklist();
  bool is_blocked(const std::string& domain) const;
};