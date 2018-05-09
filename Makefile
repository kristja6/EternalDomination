PROGRAM=medn
CC=g++
CFLAGS=-std=c++11 -g -O4
LIBS=-lm

all: compile

run: compile
	./main

objs/arguments.o: src/arguments.cpp src/arguments.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
	
objs/graph.o: src/graph.cpp src/graph.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

objs/max-flow.o: src/max-flow.cpp src/max-flow.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

objs/main.o: src/main.cpp src/cliqueTree.cpp src/cliqueTree.h src/graph.h src/graph.cpp src/arguments.h src/arguments.cpp src/blockcuttree.h src/blockcuttree.cpp src/cactus.h src/cactus.cpp | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

objs/blockcuttree.o: src/blockcuttree.cpp src/max-flow.cpp src/max-flow.h src/graph.cpp src/graph.h src/blockcuttree.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

objs/cactus.o: src/cactus.cpp src/cactus.h src/graph.cpp src/graph.h src/blockcuttree.cpp src/blockcuttree.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

objs/cliqueTree.o: src/cliqueTree.cpp src/cliqueTree.h src/graph.cpp src/graph.h src/blockcuttree.cpp src/blockcuttree.h | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

$(PROGRAM): objs/cliqueTree.o objs/arguments.o objs/graph.o objs/max-flow.o objs/blockcuttree.o objs/main.o objs/cactus.o | objs

	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

objs:
	mkdir objs

clean:
	rm -rf $(PROGRAM) objs/ 2>/dev/null

compile: $(PROGRAM)
