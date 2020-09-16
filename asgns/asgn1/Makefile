# Example makefile for CPE464 program 1
#
# 

CC = g++
CFLAGS = -g -ggdb -Wall -D_GLIBCXX_DEBUG 
#CFLAGS = -g

all:  trace

trace: trace.cpp
	$(CC) $(CFLAGS) -o trace trace.cpp -lpcap 

debug: trace
	gdb --args trace ArpTest.pcap

clean:
	rm -f trace
