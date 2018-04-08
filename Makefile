PROGRAM=main
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

objs/main.o: src/main.cpp src/graph.h src/graph.cpp src/arguments.h src/arguments.cpp | objs
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

$(PROGRAM): objs/arguments.o objs/graph.o objs/max-flow.o objs/main.o | objs
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

objs:
	mkdir objs

clean:
	rm -rf $(PROGRAM) objs/ 2>/dev/null

compile: $(PROGRAM)
