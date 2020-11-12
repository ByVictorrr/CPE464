#include <thread>
#include <string>
#include <list>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"
#include "Utils.hpp"

class Server{
    private:
        ServerConnection gateway;
        std::list<ServerThread> clients;
        ServerArgs args;

    public:
        Server(ServerArgs &args);
        void serve();
};

typedef enum STATES{FILENAME, SEND_DATA, WAITING, RECV_DATA, DONE} state_t;

class ServerThread{
    private:
        std::thread thread;
        FILE *file;
        uint32_t bufferSize;
        Window *window;
        ServerArgs args;
        ServerConnection gateway;

        ssize_t sendPacket(RCopyPacket &packet);
        /* State functions */
        state_t sendData(RCopyPacket *recvd);
        state_t waiting(RCopyPacket *recvd);
        state_t receiveData(RCopyPacket &p);

        
    public:
        ServerThread(RCopySetupPacket &setup, ServerArgs &args);
        void processRCopy(RCopySetupPacket &setup);
        void join();



};