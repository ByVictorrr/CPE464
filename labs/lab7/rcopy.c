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
#include "networks.h"
#include "cpe464.h"

#include "Packet.hpp"

#define HDR_LEN 7
#define MAXBUF 80
#define xstr(a) str(a)
#define str(a) #a

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int getData(char * buffer);
int checkArgs(int argc, char * argv[]);




int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	float errRate;
	portNumber = checkArgs(argc, argv);
	// Step 1 - get error rate
	errRate = atof(argv[3]);
	// Step 2 - call sendtoErr_init()
	sendtoErr_init(errRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);



	
	socketNum = setupUdpClientToServer(&server, argv[1], portNumber);
	
	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0; 
	char buffer[MAXBUF+1];
	
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{

		dataLen = getData(buffer);

		

		printf("Sending: %s with len: %d\n", buffer,dataLen);
	
		sendtoErr(socketNum, buffer, dataLen, 0, (struct sockaddr *) server, serverAddrLen);
		
		safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) server, &serverAddrLen);
		
		// print out bytes received
		ipString = ipAddressToString(server);
		printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), buffer);
	      
	}
} 
/*
uint8_t *createPDU(uint32_t sequenceNumber, uint8_t flag, uint8_t *payload, int dataLen){
	static uint8_t pduBuffer[HDR_LEN+MAXBUF+1];
	
	memset(pduBuffer, 0, HDR_LEN+MAXBUF+1);
	// seq (32 bits)
	sequenceNumber = htonl(sequenceNumber);
	memcpy(pduBuffer, sequenceNumber, sizeof(u_int32_t));
	// checksum
	
	return pduBuffer;	

}
*/


int getData(char * buffer)
{
	// Read in the data
	buffer[0] = '\0';
	printf("Enter the data to send: ");
	scanf("%" xstr(MAXBUF) "[^\n]%*[^\n]", buffer);
	getc(stdin);  // eat the \n
		
	return (strlen(buffer)+ 1);
}

int checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s host-name port-number error-rate\n", argv[0]);
		exit(1);
	}
	
	// Checks args and returns port number
	int portNumber = 0;

	
	portNumber = atoi(argv[2]);
		
	return portNumber;
}





