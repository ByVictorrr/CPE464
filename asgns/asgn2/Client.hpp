#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <regex>
#include <queue>
#include "safe_sys_calls.h"
#include "Utils.hpp"
//typedef uint16_t hdr_t;
/*************** Forward classes ********************************/
#include "Parsers.hpp"
#include "PacketFactory.hpp"
class CommandValidator;
class CommandParser;
class MCommandParser;
class BCommandParser;



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
        int getSocket(){return this->skt;}
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
      
};






#endif