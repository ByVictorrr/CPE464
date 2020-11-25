/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.hpp"
#include "Packet.hpp"
#include <queue>
#include "cpe464.h"
#include "Server.hpp"
#include "Args.hpp"

#define MAXBUF 80


int main ( int argc, char *argv[]  )
{
	ServerArgs args = ServerArgsParser::parse(argc, argv);

	Server server(args);
	server.serve();

	return 0;
}

