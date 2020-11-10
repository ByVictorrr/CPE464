
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
#include "Args.hpp"

#include "gethostbyname.h"

#define BACKLOG 10


class Connection{
	protected:
		int socketNumber;
		struct sockaddr_in6 remote;
	public:
		inline int getSocketNumber(){return this->socketNumber; }
		inline struct sockaddr_in6 *getRemote(){return &this->remote;}
		inline in_port_t getPort(){return this->remote.sin6_port;}
		inline void setSocketNumber(int sock){ this->socketNumber = sock;}
		inline void setPort(in_port_t port){
			this->remote.sin6_port = port; 
		}

};

class RCopyConnection: public Connection{
	private:
		public:
		RCopyConnection(RCopyArgs &args){
			this->socketNumber = setupConnection(&this->remote, args.getRemoteMachine(), args.getPort());
		}
		int setupConnection(struct sockaddr_in6 *server, const char *hostName, int portNumber)
		{
			// currently only setup for IPv4
			int socketNum = 0;
			char ipString[INET6_ADDRSTRLEN];
			uint8_t * ipAddress = NULL;

			// create the socket
			if ((socketNum = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
			{
				perror("socket() call error");
				exit(-1);
			}

			if ((ipAddress = gethostbyname6(hostName, server)) == NULL)
			{
				exit(-1);
			}

			server->sin6_port = ntohs(portNumber);
			server->sin6_family = AF_INET6;

			inet_ntop(AF_INET6, ipAddress, ipString, sizeof(ipString));
			printf("Server info - IP: %s Port: %d \n", ipString, portNumber);

			return socketNum;
		}


	

};


class ServerConnection: public Connection{
	private: 
		int udpServerSetup(int portNumber)
		{
			struct sockaddr_in6 server;
			int socketNum = 0;
			int serverAddrLen = 0;

			// create the socket
			if ((socketNum = socket(AF_INET6,SOCK_DGRAM,0)) < 0)
			{
				perror("socket() call error");
				exit(-1);
			}

			// set up the socket
			server.sin6_family = AF_INET6;    		// internet (IPv6 or IPv4) family
			server.sin6_addr = in6addr_any ;  		// use any local IP address
			server.sin6_port = htons(portNumber);   // if 0 = os picks

			// bind the name (address) to a port
			if (bind(socketNum,(struct sockaddr *) &server, sizeof(server)) < 0)
			{
				perror("bind() call error");
				exit(-1);
			}

			/* Get the port number */
			serverAddrLen = sizeof(server);
			getsockname(socketNum,(struct sockaddr *) &server,  (socklen_t *) &serverAddrLen);
			printf("Server using Port #: %d\n", ntohs(server.sin6_port));

			return socketNum;

		}


};


bool safeSelectTimeout(int socketNum, uint32_t sec, int32_t usec);


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
