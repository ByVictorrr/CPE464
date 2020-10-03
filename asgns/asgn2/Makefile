# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019
# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = g++
LDFLAGS = 
CFLAGS =  -g -Wall
# need a way to input all c files and headers
C_FILES = $(shell find . -name "*.c*")
OBJS = $(C_FILES:.c=.o)

all: .depend cclient server

server:  server.o networks.o gethostbyname6.o 
cclient:  cclient.o networks.o gethostbyname6.o readLongLine.o

# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend

%.o: %.c
	$(CC) $(CFLAGS)  -c -o $@ $<

#%.o: %.cpp
#	$(CC) $(CFLAGS)  -c -o $@ $<


%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f $(OBJS) .depend server cclient
