# udpCode makefile
# written by Hugh Smith - April 2017

CC = g++
CFLAGS= -g -Wall --std=c++11 -fPIE
LIBS+= libcpe464.2.20.a -lstdc++ -ldl

SRC = networks.c  networks.h gethostbyname.c gethostbyname.h Packet.cpp Packet.hpp
OBJS = networks.o gethostbyname.o Packet.o

all:  udpClient udpServer

udpClient: udpClient.c $(OBJS) 
	$(CC) $(CFLAGS) -o udpClient udpClient.c $(OBJS) $(LIBS)

udpServer$(FILE): udpServer.c $(OBJS)
	$(CC) $(CFLAGS) -o udpServer udpServer.c  $(OBJS) $(LIBS)

	
.c.o: $SRC
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f udpServer udpClient *.o

