/******************************************************************************
* myClient.c
*
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
#include "readLongLine.h"

#include <signal.h>

#define DEBUG_FLAG 1
#define LAMBDA(c_) ({ c_ _;})

char *sendToServer(int socketNum);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	char *input = NULL;
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	// step 1 - Exit if control c pressed
	signal(SIGINT, LAMBDA(void _(int sig_num){
		safe_free(&input);
		safe_close(&socketNum);
		exit(EXIT_SUCCESS);
	}));

	while(1){
		input = sendToServer(socketNum);
		if(strcmp(input, "exit") == 0){
			safe_free(&input);
			break;
		}
		safe_free(&input);
	}
	
	return 0;
}


char *sendToServer(int socketNum)
{
	char aChar = 0;
	int sendLen = 0;        //amount of data to send
	int inputLen = 0;
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	char *input;
	char *pdu;
	char hdr[HDR_LEN+1];
	memset(hdr, 0, HDR_LEN);
			
	// Important you don't input more characters than you have space 
prompt:
	fprintf(stdout, "Enter data: ");
	fflush(stdout);
	if((input = read_long_line(STDIN_FILENO, MAX_INPUT)) == NULL)
		goto prompt;

	
    // Because we cut off input after MAX_INPUT has been reached
	// we should only get stlren(input) < MAX_INPUT which can be represented in 2 bytes
	inputLen = strlen(input)+1;
	hdr[0] = inputLen >> 8;
	hdr[1] = (char)inputLen;
	sendLen=inputLen+HDR_LEN;  
	pdu=safe_calloc(sendLen, sizeof(char));
	memcpy(pdu, hdr, HDR_LEN);
	memcpy(pdu+HDR_LEN, input, inputLen);
	
	printf("read: %s string len: %d (including null)\n", input, sendLen);
		
	// get the length of 
	sent =  send(socketNum, pdu, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}
	safe_free(&pdu);

	printf("Amount of data sent is: %d\n", sent);
	return input;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
