# Assignment 1 - Template Makefile
# Example makefile for CPE464 program 1
#
# 

CC = g++ 
CFLAGS = -g
LDFLAGS =
LDLIBS = -lpcap
CPPFLAGS = -g -ggdb -Wall -D_GLIBCXX_DEBUG 
OBJS = trace.o checksum.o
FILE = 

all:  trace

trace.o: trace.cpp checksum.h
	gcc $(CFLAGS) -c $< -o $@


checksum.o: checksum.c checksum.h
	$(CC) $(CPPLAGS) -c $< -o $@

trace: $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJS) -lpcap


debug: trace
	gdb --args trace $(FILE)

clean:
	rm -f trace *.o
