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
    /* check to see if handle.size()*/
    if(strlen(argv[1]) > 100){
        printf("Handle name is too long\n");
        exit(EXIT_FAILURE);
    }
}

/* argv[1] = handle, argv[2] = serverName, argv[3] = port*/
int main(int argc, char * argv[])
{
	checkArgs(argc, argv);
    TCPClient client(argv[1]);
    client.login(argv[2], argv[3]);
    client.chat();
    client.logout();
	return 0;
}