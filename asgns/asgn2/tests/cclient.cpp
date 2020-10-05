#include "Client.hpp"
#include <iostream>

void checkArgs(int argc, char * argv[])
{
/* check command line arguments  */
    if (argc != 4)
    {
        printf("usage: %s handle server-name server-port \n", argv[0]);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char * argv[])
{
	
    #define DEBUG_FLAG 1
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
    TCPClient client(argv[1], argv[2], argv[3]);

    //client.connect(DEBUG_FLAG);
    // Take out busy wait ask if whenever somethings sent if it gets a recv
    client.loop();
    //client.close();
	return 0;
}