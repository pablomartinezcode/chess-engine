CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2

all:
	$(CXX) $(CXXFLAGS) main.cpp board.cpp move.cpp movegen.cpp magic.cpp -o engine
