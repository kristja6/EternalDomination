PROGRAM=main
CC=g++
CFLAGS=-std=c++11

all: compile drawing-compile

run: compile
	./main

tests: compile
	./main < 01.in

valgrind: compile
	valgrind ./main < 01.in

compile:
	#gcc -O1 -Wall -Werror -std=c99 -lm -o main main.cpp -lm
	g++ -std=c++11 -g -O4 -lm -o main main.h main.cpp

drawing-compile:
	mkdir -p states
	g++ -std=c++11 -g -O4 -lm -o drawing main.h drawingTool.cpp

draw:
	touch ./states/junk
	rm ./states/*
	./drawing < drawing.in