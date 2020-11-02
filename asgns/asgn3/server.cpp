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
#include <thread>

#include "gethostbyname.h"
#include "networks.hpp"
#include "Packet.hpp"
#include <queue>
#include "cpe464.h"
#include "Server.hpp"

#define MAXBUF 80

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);

int main ( int argc, char *argv[]  )
{
	int socketNum = 0;
	int portNumber = 0;
	float errPercent;

	// Step 0 - check args
	checkArgs(argc, argv);
	errPercent = atof(argv[1]);

	/*
	std::queue<std::thread> clients;
	*/
	portNumber = checkArgs(argc, argv);
	socketNum = udpServerSetup(portNumber);
	Server server(portNumber, errPercent);
	server.connect();
	server.serve();
	server.disconnect();

	return 0;
}

void processClient(int socketNum, float errPercent)
{
	int dataLen = 0;
	char buffer[MAXBUF + 1];
	struct sockaddr_in6 client;
	int clientAddrLen = sizeof(client);

	RCopyPacketParser parser;
	buffer[0] = '\0';
	sendtoErr_init(errPercent, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);

	while (buffer[0] != '.')
	{

		memset(buffer, 0, MAXBUF);
		//dataLen = safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);
		parser.outputPDU((uint8_t*)buffer);
		printf("Received message from client with ");
		printIPInfo(&client);

	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc != 2 || argc != 3 )
	{
		fprintf(stderr, "Usage %s <error-percent> [optional port number]\n", argv[0]);
		exit(-1);
	}

	portNumber = atoi(argv[1]);

	return portNumber;
}
