#ifndef CLIENT_H
#define CLIENT_H
#include "NetworkNodes.hpp"

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

        // Fd_set var so we can use two function's below
        int processStdIn(); // helper of recv
        size_t processSocket(); // helper of recv
        int 

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
    
};



#endif