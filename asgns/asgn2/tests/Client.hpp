#ifndef CLIENT_H
#define CLIENT_H
#include "Utils.hpp"
#include <iostream>
#include <regex>
#include <queue>
class PacketFactory;
//typedef uint16_t hdr_t;


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

        /* Inputs of a client */
        std::string readUserInput(); // helper of recv
        size_t processSocket(); // helper of recv

        /* How to parse/build messages to be sent or recv*/

    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
        void close();
        char * getHandle(){return this->handle;};
        uint8_t * getTransBuff(){return this->transBuff;};
};

class TCPClient: public Client{
    public:
        TCPClient(char *handle, char *server_name, char *port, int protocol=0);
        void connect(int debugFlag);
        // like main
        void loop();
        int send(uint16_t pkt_len){
            return safe_send(this->skt, this->transBuff, pkt_len, 0);
        }
        friend class PacketFactory;
};







#endif