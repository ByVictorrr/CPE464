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
        int processSocket(); // helper of recv

    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
        virtual void send()=0;
        virtual void recv()=0;
        virtual void close()=0;
   

};

class TCPClient: public Client{
    private:
    public:
        TCPClient(char *handle, char *server_name, char *port, int protocol=0);
        void connect(int debugFlag);

        void getInput();
        void send();
        void recv();
        void close();

        // like main
        void loop();
    
};



#endif