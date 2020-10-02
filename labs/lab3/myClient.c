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

#include <signal.h>

#define DEBUG_FLAG 1
#define LAMBDA(c_) ({ c_ _;})

void sendToServer(int socketNum, char *buf, int buf_size);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	char sendBuf[MAXBUF];
	int keepRunning = 1;
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	// step 1 - Exit if control c pressed
	signal(SIGINT, LAMBDA(void _(int sig_num){
		keepRunning = 0;
		close(socketNum);
		exit(EXIT_SUCCESS);
	}));

	while(keepRunning && strcmp(&sendBuf[2], "exit")){
		sendToServer(socketNum, sendBuf, MAXBUF);
	}
	
	return 0;
}

void sendToServer(int socketNum, char *buf, int buf_size)
{
	char aChar = 0;
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	char tot_buf[MAXBUF];
	char msg_len[2];
	memset(buf, 0, buf_size);
	memset(tot_buf, 0, MAXBUF);
	memset(msg_len, 0, 2);
			
	// Important you don't input more characters than you have space 
	printf("Enter data: ");
	while (sendLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buf[sendLen] = aChar;
			sendLen++;
		}
	}

	buf[sendLen] = '\0';
	sendLen++;  //we are going to send the null
	
	printf("read: %s string len: %d (including null)\n", buf, sendLen);
		
	// get the length of 
	sent =  send(socketNum, buf, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
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
