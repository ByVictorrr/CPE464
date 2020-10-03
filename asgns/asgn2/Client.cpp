
#include "NetworkNodes.hpp"
/* Client functions */
// Like the send helper
int Client::createPacket(int fd){
    uint8_t c;
    int place = (HDR_LEN+FLAG_LEN-1);
    int len;
    // get input
    while((read(fd, &c, sizeof(uint8_t))) > 0){
        // Case 1 - user enters 'enter' or if we ran out of space
        if(c == '\n' || place >= MAX_BUFF-1); // TODO maybe take out null
            break;
        this->transBuff[place++] = c;
    }
    // Set the size of msg
    len=place+1;
    transBuff[0] = len >> 8;
    transBuff[1] = (uint8_t)len;
    // TODO : get the flag 
    return len;
}

Client::Client(char *handle, char *server_name, char *port, int type, int protocol=0)  
: handle(handle), serverName(server_name), port(port)
{
    this->skt = safe_socket(AF_INET6, type, protocol);
    memset(recvBuff, 0, MAX_BUFF);
    memset(transBuff, 0, MAX_BUFF);
}
Client::~Client(){
    safe_close(&this->skt);
}



TCPClient::TCPClient(char *handle, char *server_name, char *port, int type, int protocol=0)  
: Client(handle, server_name, port, SOCK_STREAM, protocol){
    safe_close(&this->skt);
}

void TCPClient::connect(int debugFlag){
    uint8_t * ipAddress = NULL;
    struct sockaddr_in6 server; 
    if(debugFlag)
        printf("Connecting to server on port number %s\n", port);

    // setup the server structure
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(atoi(this->port));

    // get the address of the server 
    if ((ipAddress = getIPAddress6(serverName, &server)) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    printf("server ip address: %s\n", getIPAddressString(ipAddress));
    safe_connect(this->skt, (struct sockaddr*)&server, sizeof(server));

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString(ipAddress), atoi(this->port));
    }
}
    /* need to create methods for reading and writing */
    void TCPClient::send(){
        int len;
        uint8_t *pkt;
        memset(this->transBuff, 0, MAX_BUFF);
        len = this->createPacket(STDIN_FILENO);
        pkt = this->transBuff;
        safe_send(this->skt, pkt, len, 0);
        printf("read: %s string len: %d (including null)\n", (char*)pkt, len);
    }
    void TCPClient::recv(){
        ssize_t len;
        // recv message
        // the server disconnected
        if((len=safe_recv(this->skt, this->recvBuff, MAX_BUFF, 0)) == 0){
            exit(EXIT_FAILURE);
        }
    }

    void TCPClient::close(){
        safe_close(&this->skt);
    }


void checkArgs(int argc, char * argv[])
{
/* check command line arguments  */
if (argc != 4)
{
    printf("usage: %s handle server-name servername \n", argv[0]);
    exit(1);
}

}
