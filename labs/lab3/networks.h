
/* 	Code originally give to Prof. Smith by his TA in 1994.
	No idea who wrote it.  Copy and use at your own Risk
*/


#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#define BACKLOG 10
#define HDR_LEN 2 // bytes
#define MAX_INPUT (1 << (HDR_LEN*8)) // number of positions psso
#define TIME_IS_NULL 1
#define TIME_IS_NOT_NULL 2

#define MAXBUF 1024
// for the server side
int tcpServerSetup(int portNumber);
int tcpAccept(int server_socket, int debugFlag);

// for the client side
int tcpClientSetup(char * serverName, char * port, int debugFlag);

int selectCall(int socketNumber, int seconds, int microseconds, int timeIsNotNull);

#endif
