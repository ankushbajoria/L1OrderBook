#include "EventParser.h"
#include "Handler.h"

#include <iostream>
#include <tuple>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>

static inline uint64_t rdtsc() {
  uint32_t hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) + (uint64_t)lo;
}

int main(int argc, char** argv) {
  if (argc <= 1) {
   LOG(ERROR, "Usage: L1OrderBook file.dat");

   return -1;
  }

  using min       = boost::accumulators::tag::min;
  using max       = boost::accumulators::tag::max;
  using mean      = boost::accumulators::tag::mean;
  using quantile  = boost::accumulators::tag::tail_quantile<boost::accumulators::right>;
  using Features  = boost::accumulators::stats< min, max, mean, quantile>;

  using namespace boost::accumulators;
  std::size_t index_ = 100000;
  boost::accumulators::accumulator_set<uint64_t, Features> latencies(tag::tail<right>::cache_size = index_);

  Handler handler;

  EventParser parser;
  parser.init(argv[1]);

  bool hasNext = false; ExchMsg msg;

  std::tie(msg, hasNext) = parser.next();

  while (hasNext) {
    boost::apply_visitor(handler, msg);

    auto start = rdtsc();
    std::tie(msg, hasNext) = parser.next();
    auto end = rdtsc();

    latencies(end - start);
  }

  LOG(INFO, "Min latency:  " + std::to_string( boost::accumulators::min(latencies)));
  LOG(INFO, "Max latency:  " + std::to_string( boost::accumulators::max(latencies)));
  LOG(INFO, "Mean latency: " + std::to_string( boost::accumulators::mean(latencies)));

  LOG(INFO, "Quantile .01: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.01)));
  LOG(INFO, "Quantile .05: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.05)));
  LOG(INFO, "Quantile .10: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.1)));
  LOG(INFO, "Quantile .25: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.25)));
  LOG(INFO, "Quantile .50: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.5)));
  LOG(INFO, "Quantile .75: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.75)));
  LOG(INFO, "Quantile .90: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.90)));
  LOG(INFO, "Quantile .95: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.95)));
  LOG(INFO, "Quantile .99: " + std::to_string( boost::accumulators::quantile(latencies, quantile_probability = 0.99)));

  Logger::getInstance()->~Logger();

  return 0;
}
