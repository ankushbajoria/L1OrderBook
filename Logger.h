#pragma once

#include <atomic>
#include <iostream>
#include <queue>

#include <pthread.h>

enum LogLevel {
  INFO   = 0,
  ERROR  = 1,
};

class Logger {
 public:
  static Logger* getInstance() { return &s_logger; }

   // settor
  void setLogLevel(int level) {
    m_level = static_cast<LogLevel>(level); 
  };

   // accessor
  int  getLogLevel() const {
    return static_cast<int>(m_level);
  }

  void print(const std::string& str) {
    std::cout << str << std::endl;
  }

 private:
  LogLevel m_level = LogLevel::INFO;

 private:
  static Logger s_logger;
};

#define LOG(X, Y) \
  if (X >= Logger::getInstance()->getLogLevel()) { \
    Logger::getInstance()->print(Y); \
  }

