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


#include "networks.h"
#include <signal.h>

#define DEBUG_FLAG 1
#define LAMBDA(c_) ({ c_ _;})

int recvFromClient(int clientSocket, char *buf, int buf_len);
int checkArgs(int argc, char *argv[]);


int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	char recv_msg[MAXBUF];
	int keepRunning;
	// Configuration	
	portNumber = checkArgs(argc, argv);
	serverSocket = tcpServerSetup(portNumber);
	signal(SIGINT, LAMBDA(void _(int sig_num){
		close(serverSocket);
		exit(EXIT_SUCCESS);
	}));


	// Get a client
accept:
	clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
	memset(recv_msg, 0, MAXBUF);
	keepRunning = 1;
	// wait for client to connect
	while(keepRunning && strcmp(&recv_msg[2],"exit") != 0 ){
		keepRunning = recvFromClient(clientSocket, recv_msg, MAXBUF);
	}
	
	goto accept;
	
	return 0;
}

int recvTwoBytesPlusMsg(int clientSocket, char *buf, int buf_size)
{
	int messageLen, message;
	// Step 1 - read the 2 bytes
	if((messageLen = recv(clientSocket, buf, 2, MSG_WAITALL)) < 0){
		perror("recv call");
		exit(-1);
	// Step 2 - check to see if client disconnected
	}else if(messageLen == 0){
		close(clientSocket);
		return 0;
	}

	memset(buf, 0, buf_size);
	if((message = recv(clientSocket, buf, buf_size, 0)) < 0){
		perror("recv call");
		exit(-1);
	}
	return messageLen+message;

}

int recvFromClient(int clientSocket, char *buf, int buf_len)
{
	int messageLen = 0;
	memset(buf, 0, buf_len);
	
	// Use a time value of 1 second (so time is not null)
	while (selectCall(clientSocket, 1, 0, TIME_IS_NOT_NULL) == 0)
	{
		printf("Select timed out waiting for client to send data\n");
	}
	
	//now get the data from the client_socket (message includes null)
	messageLen = recvTwoBytesPlusMsg(clientSocket, buf, buf_len);

	printf("Message received, length: %d Data: %s\n", messageLen, buf);
	return messageLen;
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

