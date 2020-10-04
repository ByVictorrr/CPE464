#ifndef SERVER_H
#define SERVER_H

#include "NetworkNodes.hpp"
#include <map>

class Server{
    protected:
        int skt; 
        std::map<std::string, int> *clients;
        int port; // maybe remove
    public:
        Server(int port, int type, int protocol=0) ;
        ~Server(){}
        virtual void config() = 0;
};



class TCPServer: public Server{
    public:
        TCPServer(int port, int protocol=0);
        void config();
        int acceptClient(int server_socket, int debugFlag);

int 
};


#endif