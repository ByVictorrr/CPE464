# udpCode makefile
# written by Hugh Smith - April 2017
SHELL := /bin/bash
CC = g++
CFLAGS= -g -Wall --std=c++11
LIBS+= libcpe464.2.20.a

C_FILES = $(shell find . -maxdepth 1 -name "*.c*")
OBJS = Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o

rcopy: rcopy.o RCopy.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 
server: server.o Server.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 


all: .depend rcopy server

#rcopy: rcopy.o RCopy.o $(OBJS)

#server: server.o Server.o $(OBJS)

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

