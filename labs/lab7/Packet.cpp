#include "Packet.hpp"
#include <iostream>
#include <utility>
#include "checksum.h"
#include "networks.h"

// Private
struct RCopyPacketBuilder::RCopyHeader{
    uint32_t sequenceNum;
    uint16_t checksum;
    uint8_t flag;
};


int RCopyPacketBuilder::setHeader(RCopyHeader *hdr, uint32_t seqNum, uint8_t flag){
    hdr->sequenceNum = htonl(seqNum);
    hdr->checksum = 0;
    hdr->flag = flag;
    return sizeof(*hdr);
}
int RCopyPacketBuilder::setPacket(uint8_t *packet, uint8_t *payload, struct RCopyHeader *hdr, int payloadLen){
    int cursor = 0;
    uint16_t checksum;
    memset(packet, 0, MAX_PAYLOAD+sizeof(*hdr));
    memcpy(packet, hdr, (cursor=sizeof(*hdr)));
    memcpy(packet+cursor, payload, payloadLen);
    checksum = (uint16_t)in_cksum((unsigned short *)packet, cursor+payloadLen);
    memcpy(packet+4, &checksum, sizeof(checksum));

    return cursor+payloadLen;
}



// public

 std::pair<int,uint8_t*> RCopyPacketBuilder::buildPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen){
    struct RCopyHeader hdr;
    int packetLen;
    if(MAX_PAYLOAD < payloadLen){
        std::cerr << "Payload too big" << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(packet, 0, MAX_PAYLOAD+MAX_HDR);
    setHeader(&hdr, seqNum, flag);
    setPacket(packet, payload, &hdr, payloadLen);

    return std::make_pair(packetLen, packet); ///size, packet
}




