#ifndef SERVER_H
#define SERVER_H

#include "Utils.hpp"
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

};
// usually used in the client to print out the message
class PacketParser: public Packet{
    protected:
        static const int NUM_COMMANDS = 4;
	    const char COMMANDS[NUM_COMMANDS] = {
            'M',
            'B',
            'L',
            'E',
        };
        bool inCommnds(char c){
            for(int i=0; i <NUM_COMMANDS; i++){
                if(toupper(c) == COMMANDS[i]){
                    return true;
                }
            }
            return false;
        }

    public:
};

// usually used in the server to build packge and forward
class PacketBuilder: public Packet{
    public:
        uint8_t *serverBuild(uint8_t *input){

        }

};



#endif