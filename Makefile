
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fsanitize=address
LDFLAGS = -fsanitize=address
EXEC = hw5

SRC = HW5.cpp Edge.cpp Vertex.cpp Graph.cpp tinyxml2.cpp
OBJ = $(SRC:.cpp=.o)
DEPS = Edge.hpp Vertex.hpp Graph.hpp tinyxml2.h

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean

