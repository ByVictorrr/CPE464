
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
		int remoteLen;
	public:
		inline int *getSocketNumber(){return &this->socketNumber; }
		inline struct sockaddr_in6 *getRemote(){return &this->remote;}
		inline in_port_t *getPort(){return &this->remote.sin6_port;}
		inline int *getRemoteLen(){return &this->remoteLen;}
		inline void setSocketNumber(int sock){ this->socketNumber = sock;}
		inline void setPort(in_port_t port){
			this->remote.sin6_port = port; 
		}

};

class RCopyConnection: public Connection{
	private:
		public:
		RCopyConnection(const char *remoteMachine, int portNumber){
			this->socketNumber = setup(&this->remote, remoteMachine, portNumber);
			this->remoteLen = sizeof(this->remote);
		}
		int setup(struct sockaddr_in6 *remote, const char *hostName, int portNumber)
		{
			// currently only setup for IPv4
			int socketNum = 0;
			char ipString[INET6_ADDRSTRLEN];
			uint8_t * ipAddress = NULL;

			// create the socket
			if ((socketNum = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
			{
				perror("socket() call error");
				exit(EXIT_FAILURE);
			}

			if ((ipAddress = gethostbyname6(hostName, remote)) == NULL)
			{
				exit(EXIT_FAILURE);
			}

			remote->sin6_port = ntohs(portNumber);
			remote->sin6_family = AF_INET6;

			inet_ntop(AF_INET6, ipAddress, ipString, sizeof(ipString));
			printf("Server info - IP: %s Port: %d \n", ipString, portNumber);

			return socketNum;
		}

};


class ServerConnection: public Connection{
	public:
		ServerConnection(int portNum){
			this->socketNumber = setup(&this->remote, portNum);
			this->remoteLen = sizeof(struct sockaddr_in6);
		}

		int setup(struct sockaddr_in6 *remote, int portNumber)
		{
			int socketNum = 0;
			int serverAddrLen = 0;

			// create the socket
			if ((socketNum = socket(AF_INET6,SOCK_DGRAM,0)) < 0)
			{
				perror("socket() call error");
				exit(-1);
			}

			// set up the socket
			remote->sin6_family = AF_INET6;    		// internet (IPv6 or IPv4) family
			remote->sin6_addr = in6addr_any ;  		// use any local IP address
			remote->sin6_port = htons(portNumber);   // if 0 = os picks

			// bind the name (address) to a port
			if (bind(socketNum,(struct sockaddr *) remote, remoteLen) < 0)
			{
				perror("bind() call error");
				exit(-1);
			}

			/* Get the port number */
			getsockname(socketNum,(struct sockaddr *) remote,  (socklen_t *) remoteLen);
			printf("Server using Port #: %d\n", ntohs(remote->sin6_port));

			return socketNum;

		}


};


bool safeSelectTimeout(int socketNum, uint32_t sec, int32_t usec);
ssize_t safeSendToErr(int socketNum, void * buf, int len, int flags, struct sockaddr *srcAddr, int addrLen);
int safeRecvfrom(int socketNum, void * buf, int len, int flags, struct sockaddr *srcAddr, int * addrLen);

#endif
