# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019
# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = g++
LDFLAGS = 
CFLAGS =  -g -std=c++11 -Werror -ggdb -Wall -D_GLIBCXX_DEBUG 
# need a way to input all c files and headers
C_FILES = $(shell find . -name "*.c*")

all: .depend cclient server

server:  server.o Server.o gethostbyname6.o Utils.o safe_sys_calls.o
cclient:  cclient.o Client.o gethostbyname6.o Utils.o safe_sys_calls.o



# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend

%.o: %.c
	$(CC) $(CFLAGS)  -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS)  -c -o $@ $<


%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

debug: cclient
	gdb -ex 'b Client.cpp:107' --args ./cclient handle1 localhost 55537

clean:
	rm -f *.o .depend server cclient
