#include "logger.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::mutex Logger::log_mutex;
std::ofstream Logger::logfile;

void Logger::init(const std::string& filename) {
  logfile.open(filename, std::ios::app);
  if (!logfile.is_open()) {
    std::cerr << "Failed to open log file: " << filename << std::endl;
  }
}

void Logger::info(const std::string& message) {
  log("INFO", message);
}

void Logger::error(const std::string& message) {
  log("ERROR", message);
}

void Logger::log(const std::string& level, const std::string& message) {
  std::lock_guard<std::mutex> lock(log_mutex);

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream timestamped;
  timestamped << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
              << "[" << level << "] " << message;

  std::cout << timestamped.str() << std::endl;

  if (logfile.is_open()) {
    logfile << timestamped.str() << std::endl;
  }
}