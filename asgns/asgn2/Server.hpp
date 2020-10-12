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
        uint8_t Buff[MAX_BUFF];

    public:

        Server(int port, int type, int protocol=0) ;
        virtual void config() = 0;
        virtual void run() = 0;
        virtual void shutdown() = 0;

};



class TCPServer: public Server{

    private:
        void forwardListHandles(int client);
        void forwardCheckHandle(int client, uint16_t pkt_len);
        void forwardExit(int client);
        void forwardBroadcast(int client, uint16_t pkt_len);
        void forwardMulticast(int client, uint16_t pkt_len);
        void processClient(int fd);

        /* Utility functions for the map */
        bool inClients(const std::string &handle);
        int getSocket(const std::string &handle);

        int acceptClient(int server_socket, int debugFlag);

    public:
        TCPServer(int port, int protocol=0);
        ~TCPServer();
        void config();
        void run();
        void shutdown();
};




#endif