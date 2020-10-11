# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019
# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = g++
LDFLAGS = 
CFLAGS =  -g -Wall  -std=c++1y
BUILD=build
PORT=45679
PORT_PID = $(lsof -t -i tcp:45679)



# need a way to input all c files and headers
C_FILES = $(shell find . -maxdepth 1 -name "*.c*")

all: .depend cclient server

server:  $(BUILD)/server.o $(BUILD)/Server.o $(BUILD)/Utils.o $(BUILD)/gethostbyname6.o $(BUILD)/safe_sys_calls.o $(BUILD)/PacketFactory.o $(BUILD)/CommandParser.o $(BUILD)/PacketParser.o 
cclient:  $(BUILD)/cclient.o $(BUILD)/Client.o $(BUILD)/Utils.o $(BUILD)/gethostbyname6.o $(BUILD)/safe_sys_calls.o $(BUILD)/PacketFactory.o $(BUILD)/CommandParser.o $(BUILD)/PacketParser.o 

# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS)  -c -o $@ $<

$(BUILD)/%.o: %.cpp
	$(CC) $(CFLAGS)  -c -o $@ $<


%: $(BUILD)/%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 


clean:
	rm -f *.o .depend server cclient

debug_server: server
	gdb --args $(PORT) ./server

run_server: server 
	sudo lsof -i tcp:$(PORT); ./server $(PORT)

run_client: cclient
	./cclient handle1 localhost $(PORT)

debug_server: server
	sudo lsof -i tcp:$(PORT); gdb --args ./server $(PORT) 

debug_client: cclient
	gdb --args ./cclient handle1 localhost $(PORT) 

val_server: server
	valgrind --leak-check=full --show-leak-kinds=all ./server $(PORT)

val_client: cclient
	valgrind --leak-check=full --show-leak-kinds=all ./cclient handle1 localhost $(PORT)
