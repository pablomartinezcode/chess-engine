CXX := g++
CPPFLAGS := -Isrc -Isrc/core -Isrc/movegen
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 -MMD -MP

TARGET := engine

SOURCES := $(wildcard src/*.cpp src/core/*.cpp src/movegen/*.cpp)

OBJECTS := $(patsubst src/%.cpp, build/%.o, $(SOURCES))

DEPS := $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

-include $(DEPS)

.PHONY: all run clean