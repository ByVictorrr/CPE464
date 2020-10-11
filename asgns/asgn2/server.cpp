#include "Server.hpp"
#include <iostream>

#define DEBUG_FLAG 1

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	char *data=NULL;
	// Configuration	
	portNumber = checkArgs(argc, argv);
    TCPServer server(portNumber);
	// wait for client to connect
	server.config();
    server.loop();

	
	return 0;
}