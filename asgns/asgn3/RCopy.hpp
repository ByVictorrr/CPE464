#ifndef RCOPY_H_
#define RCOPY_H_
#include "Packet.hpp"
#include "Args.hpp"
#include "Utils.hpp"
#include "networks.hpp"
#include "Exception.hpp"
#include "safe_sys_calls.h"

typedef enum RCOPY_STATES{FILENAME, FILENAME_OK, RECV_DATA, OUT_OF_ORDER, DONE} state_t;


class RCopy{
    private:
        RCopyArgs args;

        Window window;
        RCopyConnection gateway;
        FILE *toFile;
        //================Two main functions===========================//
        ssize_t sendPacket(RCopyPacket &packet);
        ssize_t sendSetupPacket(RCopySetupPacket &packet);
        RCopyPacket recievePacket() throw (CorruptPacketException);
        RCopyPacket buildPacket(uint32_t seqNum, uint8_t flag);
        size_t writePacketToFile(RCopyPacket &p);

        //============= State functions ===============//
        state_t sendFileName();
        state_t receieveData();
    public:
        RCopy(RCopyArgs & cmdArgs);
        void start();
        
};
#endif