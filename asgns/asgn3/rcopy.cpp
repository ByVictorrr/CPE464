// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

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

#include "gethostbyname.h"
#include "networks.hpp"
#include "cpe464.h"

#include "Packet.hpp"
#include "Args.hpp"
#include "RCopy.hpp"
#include <iostream>

#define HDR_LEN 7
#define MAXBUF 80
#define xstr(a) str(a)
#define str(a) #a
#define FLAG 1

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int getData(char * buffer);




int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	RCopyArgs args = RCopyArgsParser::parse(argc, argv);
	sendtoErr_init(args.getErrorPercent(), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	RCopy rcopy(args);
	rcopy.start();	

	return 0;
}


