L1OrderBook : main.cpp Logger.cpp
	clang++ -o L1OrderBook Logger.cpp main.cpp -Wall -std=c++11 -lpthread -O3

clean:
	${RM} L1OrderBook
