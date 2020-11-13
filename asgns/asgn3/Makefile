# udpCode makefile
# written by Hugh Smith - April 2017
SHELL := /bin/bash
CC = g++
CFLAGS= -g -Wall --std=c++14 -pthread
LIBS+= libcpe464.2.20.a

C_FILES = $(shell find . -maxdepth 1 -name "*.c*")
OBJS = Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o


all: .depend server rcopy


rcopy: rcopy.o RCopy.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 
server: server.o Server.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 



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

run_rcopy: rcopy
	./rcopy $(shell pwd)/tests/from_files/test1 $(shell pwd)/tests/to_files/test1 10 2 .1 localhost 45678

run_server: server
	./server .1 45678

debug_rcopy: rcopy
	gdb --args ./rcopy $(shell pwd)/tests/from_files/test1 $(shell pwd)/tests/to_files/test1 10 2 .1 localhost 45678

debug_server: server
	gdb --args ./server .1 45678

