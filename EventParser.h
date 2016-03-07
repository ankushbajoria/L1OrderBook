#pragma once

#include "Logger.h"
#include "RawMsg.h"

#include <utility>

#include <cassert>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class EventParser {
  static const int MAX_PKT_SIZE = 1500;

 public:
  EventParser() = default;

  bool init(const char* filename);
  std::pair<ExchMsg, bool> next();

 private:
  int     m_fileFd            = 0;
  char*   m_buffer            = nullptr;
  char*   m_dgramBuf          = nullptr;
  size_t  m_numRemainingEvts  = 0;

  char*   m_data = nullptr;
  size_t  m_offset = 0;
  size_t  m_fileSize = 0;

  size_t  m_totalEvts = 0;
};

inline
bool
EventParser::init(const char* filename) {
  auto fd = ::open(filename, O_RDONLY);

  if (fd == -1) {
    // Error condition, file not present?
    LOG(ERROR, std::string("EventParser::init failed to open file ")
        + filename + ". errno" + std::to_string(errno));

    return false;
  }

  // get file size
  struct stat s;
  ::fstat(fd, & s);
  m_fileSize = s.st_size;

  auto ret = ::mmap(NULL, m_fileSize, PROT_READ, MAP_SHARED, fd, 0);
  if (ret == MAP_FAILED) {
    LOG(ERROR, "EventParser::init mmap failed");
    return false;
  }

  m_data = reinterpret_cast<char*>(ret);

  m_fileFd = fd;
  LOG(INFO, "EventParser::init successfully loaded " + std::string(filename));

  return true;
}

inline
std::pair<ExchMsg, bool>
EventParser::next() {
  if (m_numRemainingEvts == 0) {
    if (m_offset >= m_fileSize) {
      LOG(INFO, "EventParser::next completed, totalEvts:" + std::to_string(m_totalEvts));
      return std::make_pair(ExchMsg(), false);
    }

    m_buffer = m_data + m_offset;

    // we are not currently handling the scenario of read less than PacketHeader
    auto pktHdr = reinterpret_cast<const PacketHeader*>(m_buffer);
    auto pktLen = ::ntohs(pktHdr->length);

    m_offset += pktLen;

    m_numRemainingEvts  = ::ntohs(pktHdr->numUpdates);
    m_totalEvts         += m_numRemainingEvts;
    m_dgramBuf          = m_buffer + sizeof(PacketHeader);
  }

  auto hdr = reinterpret_cast<Header*>(m_dgramBuf);

  switch (static_cast<MsgType>(hdr->type)) {
    case MsgType::QUOTE:
    {
      auto quoteMsg = reinterpret_cast<Quote*>(m_dgramBuf);
      m_dgramBuf += ::ntohs(hdr->length);
      m_numRemainingEvts--;

      return std::make_pair(ExchMsg(*quoteMsg), true);
    } break;
    case MsgType::TRADE:
    {
      auto tradeMsg = reinterpret_cast<Trade*>(m_dgramBuf);
      m_dgramBuf += ::ntohs(hdr->length);
      m_numRemainingEvts--;

      return std::make_pair(ExchMsg(*tradeMsg), true);
    } break;
    default:
    {
      LOG(ERROR, "EventParser::next unknown msgtype " + std::to_string(hdr->type));
    }
  }

  return std::make_pair(ExchMsg(), true);
}
