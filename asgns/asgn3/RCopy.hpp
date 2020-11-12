#ifndef RCOPY_H_
#define RCOPY_H_
#include "Packet.hpp"
#include "Args.hpp"
#include "Utils.hpp"
#include "networks.hpp"
#include "Exception.hpp"
#include "safe_sys_calls.h"

typedef enum RCOPY_STATES{FILENAME, FILENAME_OK, RECV_DATA, DONE} state_t;


class RCopy{
    private:
        Window window;
        RCopyConnection gateway;
        RCopyArgs args;
        FILE *toFile;
        //================Two main functions===========================//
        ssize_t sendPacket(RCopyPacket &packet);
        RCopyPacket recievePacket() throw (CorruptPacketException);
        RCopyPacket buildRR(uint32_t seqNum);
        RCopyPacket buildSEJ(uint32_t seqNum);
        size_t writePacketToFile(RCopyPacket &p);

        //============= State functions ===============//
        state_t sendFileName();
        state_t recvData();
    public:
        RCopy(RCopyArgs & cmdArgs);
        void start();
        
};
#endif