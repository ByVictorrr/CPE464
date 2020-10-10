#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <regex>
#include <queue>
#include "safe_sys_calls.h"
//typedef uint16_t hdr_t;

class PacketFactory;
class PacketParser;
class CommandParser;
class CommandValidator;
#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10

// usually used in the client to print out the message

class Client{
    protected:
        /* Data that is needed as client */
        int skt; 
        char *port;
        char *handle;
        char *serverName;

        /* Chat-protocol Buffers */
        uint8_t recvBuff[MAX_BUFF];
        uint8_t transBuff[MAX_BUFF];
    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
        void close();
        char * getHandle(){return this->handle;};
        uint8_t * getTransBuff(){return this->transBuff;};
};

class TCPClient: public Client{
    private:
        void processUserInput();
        void processSocket();
        uint32_t numHandles;
    public:
        TCPClient(char *handle, char *server_name, char *port, int protocol=0);
        void connect(int debugFlag);
        void loop();
      
        friend class PacketFactory;
        friend class PacketParser;
};






#endif