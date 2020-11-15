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


PORT=45680
FROM_DIR=$(shell pwd)/tests/from_files
TO_DIR=$(shell pwd)/tests/to_files
REMOTE_MACHINE=localhost

# ./rcopy <from-file> <to-file> <buff-size> <err-per> <win-size> <remote-name> <remote-port>

#===========Test 1=================


WIND_SIZE=5
BUFF_SIZE=1000
RCOPY_ERR_PER=.15
SERVER_ERR_PER=$(RCOPY_ERR_PER)
FROM_TEST=long_binary
TO_TEST=$(FROM_TEST)

test_rcopy: rcopy
											# <buff-size> <err-per> <wind-size>
	./rcopy $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) $(BUFF_SIZE) $(RCOPY_ERR_PER) $(WIND_SIZE) $(REMOTE_MACHINE) $(PORT)
	diff $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) 

test_server: server
	./server $(SERVER_ERR_PER) $(PORT)

debug_rcopy: rcopy
	gdb --args ./rcopy $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) $(BUFF_SIZE) $(RCOPY_ERR_PER) $(WIND_SIZE) $(REMOTE_MACHINE) $(PORT)

debug_server: server
	gdb --args ./server $(SERVER_ERR_PER) $(PORT)

