#include "NetworkNodes.hpp"
#include "Client.hpp"

/***************** Client functions ****************************/
// TODO : work on processing flag
uint16_t Client::processStdIn(){
    uint8_t c;
    //int place = (HDR_LEN+FLAG_LEN-1);
    int place = 0;
    int len;
    // get input
    while((read(STDIN_FILENO, &c, sizeof(uint8_t))) > 0){
        // Case 1 - user enters 'enter' or if we ran out of space
        if(c == '\n' || place >= (MAX_BUFF+HDR_LEN+FLAG_LEN-1)); // TODO maybe take out null
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
size_t Client::processSocket(){
    return (size_t)safe_recv(this->skt, this->recvBuff, MAX_BUFF, 0);
}

// Assume data doesnt include header 
void TCPClient::createPacket(uint8_t *data, uint16_t len, uint8_t flag){
    this->transBuff[0] = len >> 8;
    this->transBuff[1] = (uint8_t)len;
    this->transBuff[2] = flag;
    memcpy(this->transBuf+3, data, len);
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
void Client::close(){
    safe_close(&this->skt);
}


TCPClient::TCPClient(char *handle, char *server_name, char *port, int protocol=0)  
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

    // TODO: create packet check to see if handle is taken
    safe_send(this->skt, this->handle, );

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString(ipAddress), atoi(this->port));
    }
}



    void TCPClient::loop(){

        ssize_t recv_len;
        ssize_t stdin_len;
        fd_set input;

start:
        FD_ZERO(&input);
        FD_SET(STDIN_FILENO, &input);
        FD_SET(this->skt, &input);
        memset(this->recvBuff, 0, MAX_BUFF);
        memset(this->transBuff, 0, MAX_BUFF);
        std::cout << "$: ";

        selectCall(this->skt, 0, 0,TIME_IS_NOT_NULL);
        // Case 1 - something has been writen to the socket
        if(FD_ISSET(this->skt, &input)){
            // if recv a end aka recv_len = 0
            if((recv_len = processSocket()) == 0)
                goto end;
            else{
                // print out message
                std::cout << "\n" << this->recvBuff << std::endl;
            }
        }
        // Case 2 - something has been writen to stdin
        if(FD_ISSET(STDIN_FILENO, &input)){
           stdin_len = processStdIn(); 
           std::cout << std::endl;
           safe_send(this->skt, this->transBuff, sizeof(transBuff), 0);
        }
        goto start;
end:
    }
   

