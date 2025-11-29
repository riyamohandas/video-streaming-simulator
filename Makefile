CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
TARGET = streamer
SRC = simulator.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: cleans
