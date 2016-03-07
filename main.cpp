#include "EventParser.h"
#include "Handler.h"

#include <iostream>
#include <tuple>

int main(int argc, char** argv) {
  if (argc <= 1) {
   LOG(ERROR, "Usage: L1OrderBook file.dat");

   return -1;
  }

  Handler handler;

  EventParser parser;
  parser.init(argv[1]);

  bool hasNext = false; ExchMsg msg;

  std::tie(msg, hasNext) = parser.next();

  while (hasNext) {
    boost::apply_visitor(handler, msg);

    std::tie(msg, hasNext) = parser.next();
  }

  return 0;
} 
