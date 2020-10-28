#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>


class RCopyPacketBuilder{
    private:
        // Defintions
        struct RCopyHeader;
        // constants var
        #define MAX_HDR 11
        #define MAX_PAYLOAD 1400

        static uint8_t packet[MAX_HDR+MAX_PAYLOAD];
        // Helper functions
        static int setHeader(RCopyHeader *hdr, uint32_t seqNum, uint8_t flag);
        static int setPacket(uint8_t *packet, uint8_t *payload, struct RCopyHeader *hdr, int payloadLen);
        RCopyPacketBuilder(){}
    public:
        // <size, packet>
        static std::pair<int,uint8_t*> buildPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen);
};

#endif