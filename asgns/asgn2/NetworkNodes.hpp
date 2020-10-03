#ifndef NETWORK_NODES_H
#define NETWORK_NODES_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"

#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10

class Server{
    protected:
        int skt; 
        int *client_sks;
        int port; // maybe remove
    public:
        Server(int max_clients, int port, int type, int protocol=0) ;
        ~Server(){}
        virtual void config() = 0;
};



class TCPServer: public Server{
    public:
        TCPServer(int max_clients, int port, int protocol=0);
        void config();
        int acceptClient(int server_socket, int debugFlag);

};

class Client{
    protected:
        int skt; 
        char *port;
        char *handle;
        char *serverName;
        // Buffers
        uint8_t recvBuff[MAX_BUFF];
        uint8_t transBuff[MAX_BUFF];
        //int findFlag()

        int createPacket(int fd);

    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
   

};

class TCPClient: public Client{
    private:
    public:
        TCPClient(char *handle, char *server_name, char *port, int type, int protocol=0);
        void connect(int debugFlag);
        void send();
        void recv();
        void close();
    
};



#endif