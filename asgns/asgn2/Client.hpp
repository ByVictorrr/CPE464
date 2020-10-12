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
        char *handle;
        int skt; 
        /* Chat-protocol Buffers */
        uint8_t Buff[MAX_BUFF];
    public:
        Client(char *handle, int type, int protocol=0);
        char * getHandle(){return this->handle;};
        uint8_t * getBuff(){return this->Buff;};
        int getSocket(){return this->skt;}
};

class TCPClient: public Client{
    private:
        void forwardMCommand(std::string &input);
        void forwardBCommand(std::string &input);
        void forwardLCommand();
        void forwardECommand();
        void forwardUserInput();

        uint16_t readSocket();
        void processSocket();
        
    public:
        TCPClient(char *handle, int protocol=0);
        ~TCPClient();
        void login(char *serverName, char *port);
        void chat();
        void logout();
      
};






#endif