#pragma once

#include "RawMsg.h"
#include <netinet/in.h>

#include <queue>

struct Handler : public boost::static_visitor<void> {
  template <typename T>
  void operator() (T)
  {
  }

  char m_buffer[128];
};

#if 0
template <>
inline void
Handler::operator() <Quote> (Quote msg) {
  std::string str = "Quote: ";
  str += "sym:"   + std::string(msg.symbol, 5)  + " "
         "level:" + std::to_string(::ntohs(msg.level))   + " "
         "qty:"   + std::to_string(::ntohs(msg.qty))     + " "
         "px:"    + std::to_string(msg.px);

  LOG(INFO, str);
}
#endif

template <>
inline void
Handler::operator() <Trade> (Trade msg) {
  std::string str = "Trade: ";
  str += "sym:"   + std::string(msg.symbol, 5)  + " "
         "qty:"   + std::to_string(::ntohs(msg.qty))     + " "
         "px:"    + std::to_string(msg.px);

  LOG(INFO, str);
}

#if 0
template <>
inline void
Handler::operator() <Trade> (Trade msg) {
  ::memset((void*)m_buffer, 0, sizeof(m_buffer));
  snprintf(m_buffer, sizeof(m_buffer), "%d %5s @ %.2f", ::ntohs(msg.qty), msg.symbol, msg.px);  
  LOG(INFO, m_buffer);
}
#endif
