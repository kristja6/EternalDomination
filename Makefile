PROGRAM=main
CC=g++
CFLAGS=-std=c++11

all: compile

run: compile
	./main

valgrind: compile
	valgrind ./main < 01.in

compile:
	g++ -std=c++11 -g -O4 -lm -o main max-flow.h max-flow.cpp graph.h graph.cpp main.cpp
