# udpCode makefile
# written by Hugh Smith - April 2017
SHELL := /bin/bash
CC = g++
CFLAGS= -g -Wall -static-libstdc++ --std=c++11 -Wno-deprecated
LIBS+= libcpe464.2.20.a



all: server rcopy


rcopy: rcopy.o RCopy.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 
server: server.o Server.o Packet.o gethostbyname.o networks.o safe_sys_calls.o Args.o Utils.o 

Args.o: Args.cpp Args.hpp
Packet.o: Packet.cpp networks.hpp cpe464.h Args.hpp gethostbyname.h \
 safe_sys_calls.h Packet.hpp Exception.hpp
RCopy.o: RCopy.cpp RCopy.hpp Packet.hpp Exception.hpp cpe464.h \
 networks.hpp Args.hpp gethostbyname.h safe_sys_calls.h Utils.hpp
Server.o: Server.cpp networks.hpp cpe464.h Args.hpp gethostbyname.h \
 safe_sys_calls.h Packet.hpp Exception.hpp Utils.hpp Server.hpp
Utils.o: Utils.cpp Utils.hpp Packet.hpp Exception.hpp cpe464.h \
 networks.hpp Args.hpp gethostbyname.h safe_sys_calls.h
gethostbyname.o: gethostbyname.c gethostbyname.h
networks.o: networks.cpp cpe464.h networks.hpp Args.hpp gethostbyname.h \
 safe_sys_calls.h
rcopy.o: rcopy.cpp gethostbyname.h networks.hpp cpe464.h Args.hpp \
 safe_sys_calls.h Packet.hpp Exception.hpp RCopy.hpp Utils.hpp
safe_sys_calls.o: safe_sys_calls.c gethostbyname.h safe_sys_calls.h
server.o: server.cpp gethostbyname.h networks.hpp cpe464.h Args.hpp \
 safe_sys_calls.h Packet.hpp Exception.hpp Server.hpp Utils.hpp




%.o: %.c
	$(CC) $(CFLAGS)  -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS)  -c -o $@ $<


%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f server rcopy *.o .depend


PORT=49628
FROM_DIR=$(shell pwd)/tests/from_files
TO_DIR=$(shell pwd)/tests/to_files
REMOTE_MACHINE=unix1.csc.calpoly.edu

# ./rcopy <from-file> <to-file> <buff-size> <err-per> <win-size> <remote-name> <remote-port>

#===========Test 1=================


WIND_SIZE=1000
BUFF_SIZE=1000
RCOPY_ERR_PER=.2
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

