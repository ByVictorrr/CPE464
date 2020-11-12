
// Hugh Smith April 2017
// Network code to support TCP/UDP client and server connections

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.hpp"
#include "gethostbyname.h"
#include "safe_sys_calls.h"


/****************Utility functions****************************/
// Never blocks
bool safeSelectTimeout(int socketNum, uint32_t sec, int32_t usec){
	fd_set fd;
	struct timeval timeout = {.tv_sec= sec, .tv_usec=usec};
	FD_ZERO(&fd);
	FD_SET(socketNum, &fd);
	safe_select(socketNum+1, &fd, NULL, NULL, &timeout);
	if(FD_ISSET(socketNum, &fd)){
		return true;
	}else{
		return false;
	}
}
int safeRecvfrom(int socketNum, void * buf, int len, int flags, struct sockaddr_in *srcAddr, int * addrLen)
{
	int returnValue = 0;
	if ((returnValue = recvfrom(socketNum, buf, (size_t) len, 
								flags, (struct sockaddr*)srcAddr, 
								(socklen_t *) addrLen)) < 0)
	{
		perror("recvfrom: ");
		exit(-1);
	}

	return returnValue;
}

ssize_t safeSendToErr(int socketNum, void * buf, int len, int flags, struct sockaddr_in *srcAddr, int addrLen)
{
	ssize_t sendLen;
	if((sendLen = sendtoErr(socketNum, buf, len, flags, (struct sockaddr*)srcAddr, addrLen)) < 0){
		perror("ERROR from sendtoErr");
		exit(EXIT_FAILURE);
    }

	return sendLen;
}



/******************setup functions for server/rcopy connection*******************************/
int RCopyConnection::setup(struct sockaddr_in6 *remote, const char *hostName, int portNumber)
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



int ServerConnection::setup(struct sockaddr_in6 *remote, int portNumber)
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



