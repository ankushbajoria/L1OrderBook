#pragma once

#include <boost/lockfree/spsc_queue.hpp>

#include <chrono>
#include <iostream>

#include <thread>

enum LogLevel {
  INFO   = 0,
  ERROR  = 1,
};

class Logger {
 public:
  static Logger* getInstance() { return &s_logger; }

  Logger(void);
  ~Logger();

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

  void enqueue(std::string str);

  void run();

 private:
  LogLevel m_level = LogLevel::INFO;
  boost::lockfree::spsc_queue<std::string, boost::lockfree::capacity<65536>> m_queue;

  std::thread   m_thread;
  volatile bool m_running = false;

 private:
  static Logger s_logger;
};

#define LOG(X, Y) \
  if (X >= Logger::getInstance()->getLogLevel()) { \
    Logger::getInstance()->enqueue(Y); \
  }

inline
Logger::Logger() {
  // m_thread  = std::thread(&Logger::run, this);
  m_running = true;
}

inline
Logger::~Logger() {
  m_running = false;

  if (m_thread.joinable()) {
    m_thread.join();
  }
}

inline
void
Logger::enqueue(std::string str)
{
  m_queue.push(str);
}

inline
void
Logger::run() {
  while (m_running || m_queue.read_available()) {
    std::string str;
    if (m_queue.pop(str)) {
      print(str);
    } else {
      // may be sleep
    }
  }
}
