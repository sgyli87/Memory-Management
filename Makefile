# CSE374 22WI, HW6
# Author: Yang Li
# Date 12/5/2022

CC = gcc
CARGS = -Wall -std=c11

all: bench

# basic build
bench: bench.o getmem.o freemem.o mem_utils.o
	     gcc -Wall -std=c11 -g -o bench bench.o getmem.o freemem.o mem_utils.o

# object files
# fill in your individual object targets here
getmem.o: getmem.c mem.h
				gcc -Wall -std=c11 -g -c getmem.c

freemem.o: freemem.c mem.h
				gcc -Wall -std=c11 -g -c freemem.c

mem_utils.o: mem_utils.c mem_impl.h
				gcc -Wall -std=c11 -g -c mem_utils.c

bench.o: bench.c mem.h mem_impl.h
				gcc -Wall -std=c11 -g -c bench.c

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
