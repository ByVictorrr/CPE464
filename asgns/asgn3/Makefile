# udpCode makefile
# written by Hugh Smith - April 2017
SHELL := /bin/bash
CC = g++
CFLAGS= -g -Wall --std=c++11
LIBS+= libcpe464.2.20.a

C_FILES = $(shell find . -maxdepth 1 -name "*.c*")
OBJS = Packet.o gethostbyname.o networks.o



all: .depend rcopy server

rcopy: rcopy.o $(OBJS)

server: server.o $(OBJS)

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend

%.o: %.c
	$(CC) $(CFLAGS)  -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS)  -c -o $@ $<


%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f server rcopy *.o .depend

debug_rcopy: rcopy
	gdb --args ./rcopy localhost 60144 .9
