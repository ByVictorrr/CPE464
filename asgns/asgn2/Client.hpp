#ifndef CLIENT_H
#define CLIENT_H
#include "Utils.hpp"

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
        uint16_t processUserInput(); // helper of recv
        size_t processSocket(); // helper of recv

        /* How to parse/build messages to be sent or recv*/
        PacketBuilder packetBuilder;

    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
        void close();
};

class TCPClient: public Client{
    private:
    public:
        TCPClient(char *handle, char *server_name, char *port, int protocol=0);
        void connect(int debugFlag);
        // like main
        void loop();
    
        friend PacketBuilder;
};



#endif