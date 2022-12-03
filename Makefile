# Makefile for mem memory system, CSE374 22WI


CC = gcc
CARGS = -Wall -std=c11

all: bench

# basic build
bench: bench.o memory.o mem_utils.o
	$(CC) $(CARGS) -o bench $^

# object files
# fill in your individual object targets here


# You can use these examples for other types of builds
# make sure you know what they do.
noassert: CARGS += -D NDEBUG
noassert: bench

debug: CARGS += -g
debug: bench

test: debug
	./bench 10 50

clean:
	rm -f bench *.o *~ 
