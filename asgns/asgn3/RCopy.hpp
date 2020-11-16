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
        RCopyACKPacket buildACKPacket(uint32_t seqNum, uint8_t flag);
        ssize_t sendSetupPacket(RCopySetupPacket &packet);
        ssize_t sendACKPacket(RCopyACKPacket &packet);

        RCopyPacket recievePacket() throw (CorruptPacketException);
        size_t writePacketToFile(RCopyPacket &p);

        state_t fillHoles();

        //============= State functions ===============//
        state_t sendFileName();
        state_t receieveData();
    public:
        RCopy(RCopyArgs & cmdArgs);
        void start();
        
};
#endif