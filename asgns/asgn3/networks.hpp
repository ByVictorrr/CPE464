
// 	Writen - HMS April 2017
//  Supports TCP and UDP - both client and server


#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "cpe464.h"


#define BACKLOG 10


class Connection{
	private:
		uint32_t socketNumber;
		struct sockaddr_in6 remote;
	public:
		inline uint32_t getSocketNumber(){return this->socketNumber; }
		inline struct sockaddr_in6 *getRemote(){return &this->remote;}
		// neeed set functions

};

class Gateway{
	private:
	public:
		// TODO : after filename packet we know the buffer-size
		int recvFrom(int sktNum, RCopyPacket &packet, Connection &con){
			int ret;
			// Step 1 - recieve the packet, it also changes conn to the correct port
			if(safeRecvfrom(sktNum, packet.packet, 1 /*TODO: len of*/ , 0, con.getRemote(), sizeof(*con.getRemote()) < 0){
				perror("recvFrom error");
				exit(EXIT_FAILURE);
			}
			// Step 2 - check crc value
			if(in_cksum(packet.packet, ) == 0){
				return -1; //CRC error;
			}
		}
};


//Safe sending and receiving
int safeRecv(int socketNum, void * buf, int len, int flags);
int safeSend(int socketNum, void * buf, int len, int flags);
int safeRecvfrom(int socketNum, void * buf, int len, int flags, struct sockaddr *srcAddr, int * addrLen);
int safeSendto(int socketNum, void * buf, int len, int flags, struct sockaddr *srcAddr, int addrLen);

// for the server side
int tcpServerSetup(int portNumber);
int tcpAccept(int server_socket, int debugFlag);
int udpServerSetup(int portNumber);

// for the client side
int tcpClientSetup(char * serverName, char * port, int debugFlag);
int setupUdpClientToServer(struct sockaddr_in6 *server, char * hostName, int portNumber);


#endif
