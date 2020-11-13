
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
#include "safe_sys_calls.h"

#define BACKLOG 10


class Connection{
	protected:
		int socketNumber;
		struct sockaddr_in6 remote;
		int remoteLen;
	public:
		inline int getSocketNumber(){return this->socketNumber; }
		inline struct sockaddr_in6 *getRemote(){return &this->remote;}
		inline in_port_t getPort(){return this->remote.sin6_port;}
		inline int *getRemoteLen(){return &this->remoteLen;}
		inline void setSocketNumber(int sock){ this->socketNumber = sock;}
		inline void setPort(in_port_t port){this->remote.sin6_port = port;}
		inline void tearDown(){ safe_close(this->socketNumber);}

};

class RCopyConnection: public Connection{
	private:
		public:
		RCopyConnection(const char *remoteMachine, int portNumber){
			this->socketNumber = setup(&this->remote, remoteMachine, portNumber);
			this->remoteLen = sizeof(this->remote);
		}
		int setup(struct sockaddr_in6 *remote, const char *hostName, int portNumber);

};


class ServerConnection: public Connection{
	public:
		ServerConnection(int portNum){
			this->remoteLen = sizeof(struct sockaddr_in6);
			this->socketNumber = setup(&this->remote, portNum);
		}
		int setup(struct sockaddr_in6 *remote, int portNumber);



};


bool safeSelectTimeout(int socketNum, uint32_t sec, int32_t usec);
ssize_t safeSendToErr(int socketNum, void * buf, int len, int flags, struct sockaddr_in6 *srcAddr, int addrLen);
int safeRecvfrom(int socketNum, void * buf, int len, int flags, struct sockaddr_in6 *srcAddr, int * addrLen);

#endif
