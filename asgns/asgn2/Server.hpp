#ifndef SERVER_H
#define SERVER_H

#include "Parsers.hpp"
#include "Utils.hpp"
#include "safe_sys_calls.h"
#include <algorithm>
#include <map>

class Server{
    protected:
        int skt; 
        std::map<int, std::string> clients;
        int port; // maybe remove
        uint8_t recvBuff[MAX_BUFF];
        uint8_t transBuff[MAX_BUFF];

    public:

        bool inClients(const std::string &handle);
        int getSocket(const std::string &handle);
        Server(int port, int type, int protocol=0) ;
        ~Server();
        virtual void config() = 0;


};



class TCPServer: public Server{

    void processClient(int fd);
    public:
        TCPServer(int port, int protocol=0);
        void config();
        int acceptClient(int server_socket, int debugFlag);
        void loop();

};




#endif