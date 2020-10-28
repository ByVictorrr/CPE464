#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <utility>

#define HDR_LEN 7
#define MAX_PAYLOAD_LEN 1400

class RCopyPacket{
    protected:
        struct RCopyHeader;
        static uint8_t packet[HDR_LEN+MAX_PAYLOAD_LEN];
};

class RCopyPacketBuilder: public RCopyPacket{
    private:
        // Helper functions
        static int setHeader(RCopyHeader *hdr, uint32_t seqNum, uint8_t flag);
        static int setPacket(uint8_t *packet, uint8_t *payload, struct RCopyHeader *hdr, int payloadLen);
        RCopyPacketBuilder(){}
    public:
        // <size, packet>
        static std::pair<int,uint8_t*> buildPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen);
};

class RCopyPacketParser: public RCopyPacket{
    private:
        // isError() : checksum
    public:
        // get fields
        void outputPDU(uint8_t *PDU);

};
#endif