#pragma once

#include <boost/variant.hpp>

#include <inttypes.h>

enum class MsgType {
  QUOTE = 'Q',
  TRADE = 'T',
};

struct PacketHeader {
  uint16_t length;
  uint16_t numUpdates;
} __attribute__((packed));

struct Header {
  uint16_t length;
  uint8_t  type;
} __attribute__((packed));

struct Quote : public Header {
  char      symbol[5];
  uint16_t  level;
  uint16_t  qty;
  double    px;
  // varlength dynamic data; ignored
} __attribute__((packed));

struct Trade : public Header {
  char      symbol[5];
  uint16_t  qty;
  double    px;
} __attribute__((packed));

typedef boost::variant<
  Quote,
  Trade> ExchMsg;
