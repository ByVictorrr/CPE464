#ifndef SERVER_H
#define SERVER_H

#include "NetworkNodes.hpp"

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


#endif