#ifndef SERVER_H_
#define SERVER_H_
#include <thread>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"
#include "Utils.hpp"
#include "Args.hpp"

class ServerArgs;
class ServerConnection;
class RCopySetupPacket;
class ReadEOFException;

class Server{
    private:
        ServerConnection gateway;
        ServerArgs args;

    public:
        Server(ServerArgs &args);
        void serve();
};

typedef enum STATES{FILENAME, SEND_DATA, WAITING, RECV_DATA, DONE} state_t;

class ServerThread{
    private:
        ServerThreadConnection gateway;
        Window *window;
        uint32_t bufferSize; 
        FILE *file;

        /* utils */  
        void readFile(uint8_t *payload) throw (ReadEOFException);
        RCopyPacket buildDataPacket(uint32_t seqNum) throw (ReadEOFException);

        ssize_t sendPacket(RCopyPacket &packet);
        /* State functions */
        state_t sendData();
        state_t waiting();
        state_t receiveData();

        
    public:
        ServerThread(RCopySetupPacket setup, ServerConnection &c);
        void processRCopy(RCopySetupPacket setup);
        void join();
};
#endif