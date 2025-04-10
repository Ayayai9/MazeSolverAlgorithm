.default: all

all: mazeRunner

clean:
	rm -f build/intermediate/*.o build/mazeRunner

mazeRunner: build/intermediate/Maze.o build/intermediate/Agent.o build/intermediate/mazeRunner.o
	g++ -Wall -Werror -std=c++17 -g -O -o build/$@ $^ -lmcpp

build/intermediate/%.o: src/%.cpp
	mkdir -p build/intermediate
	g++ -Wall -Werror -std=c++17 -g -O -c $< -o $@