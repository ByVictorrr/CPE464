/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

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

#include "readLongLine.h"

#include "networks.h"
#include <signal.h>

#define DEBUG_FLAG 1
#define LAMBDA(c_) ({ c_ _;})

char *recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);


int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	char *data=NULL;
	// Configuration	
	portNumber = checkArgs(argc, argv);
	serverSocket = tcpServerSetup(portNumber);
	signal(SIGINT, LAMBDA(void _(int sig_num){
		safe_close(&serverSocket);
		exit(EXIT_SUCCESS);
	}));


	// Get a client
accept:
	clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
	// wait for client to connect
	while(1){
		if((data = recvFromClient(clientSocket)) == NULL){
			safe_close(&clientSocket);
			break;
		}else if(strcmp(data, "exit") == 0){
			safe_close(&clientSocket);
			safe_free(&data);
			break;
		}
		safe_free(&data);
	}

	
	goto accept;
	
	return 0;
}

char *recvTwoBytesPlusMsg(int clientSocket)
{
	char hdr[HDR_LEN];
	int inputLen;
	int isConnected;
	char *data;
	// Step 1 - read the 2 bytes
	if((isConnected=recv(clientSocket, hdr, HDR_LEN, MSG_WAITALL)) < 0){
		perror("recv call");
		exit(-1);
	// Step 2 - check to see if client disconnected
	}else if(!isConnected){
		return NULL;
	}
	inputLen = (hdr[1] << 8) | hdr[0];
	data = safe_calloc(inputLen, sizeof(char));
	if(recv(clientSocket, data, inputLen, 0) < 0){
		perror("recv call");
		exit(-1);
	}
	return data;
}

char *recvFromClient(int clientSocket)
{
	char *data;
	int pduLen;
	// Use a time value of 1 second (so time is not null)
	while (selectCall(clientSocket, 1, 0, TIME_IS_NOT_NULL) == 0)
	{
		printf("Select timed out waiting for client to send data\n");
	}
	
	//now get the data from the client_socket (message includes null)
	if((data = recvTwoBytesPlusMsg(clientSocket)) == NULL){
		return NULL;
	}

	pduLen=strlen(data)+1+HDR_LEN;

	printf("Message received, Recv len %d, Header Len: %d, Message: %s\n", pduLen, HDR_LEN, data);

	return data;
}

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

