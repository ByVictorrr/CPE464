#include "Packet.hpp"
#include <iostream>
#include <utility>
#include "networks.h"
#include "cpe464.h"

/**************RCopyPacket******************/
struct RCopyPacket::RCopyHeader{
    uint32_t sequenceNum;
    uint16_t checksum;
    uint8_t flag;
};

uint8_t RCopyPacket::packet[HDR_LEN+MAX_PAYLOAD_LEN];

/************RCopyPacketBuilder*****************/
int RCopyPacketBuilder::setHeader(RCopyHeader *hdr, uint32_t seqNum, uint8_t flag){
    hdr->sequenceNum = htonl(seqNum);
    hdr->checksum = 0;
    hdr->flag = flag;
    return sizeof(*hdr);
}
int RCopyPacketBuilder::setPacket(uint8_t *packet, uint8_t *payload, struct RCopyHeader *hdr, int payloadLen){
    int cursor = 0;
    uint16_t checksum;
    memcpy(packet, hdr, (cursor=HDR_LEN));
    memcpy(packet+cursor, payload, payloadLen);
    checksum = (uint16_t)in_cksum((unsigned short *)packet, cursor+payloadLen);
    memcpy(packet+4, &checksum, sizeof(checksum));

    return cursor+payloadLen;
}

 std::pair<int,uint8_t*> RCopyPacketBuilder::buildPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen){
    struct RCopyHeader hdr;
    int packetLen = payloadLen+HDR_LEN;
    if(MAX_PAYLOAD_LEN < payloadLen){
        std::cerr << "Payload too big" << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(packet, 0, MAX_PAYLOAD_LEN+HDR_LEN);
    setHeader(&hdr, seqNum, flag);
    setPacket(packet, payload, &hdr, payloadLen);

    return std::make_pair(packetLen, packet); ///size, packet
}


/**********************RCopyPacketParser*************************/
void RCopyPacketParser::outputPDU(uint8_t *PDU){

    std::string payload = std::string((char*)(HDR_LEN+PDU));
    if(in_cksum((unsigned short*)PDU, payload.size()+HDR_LEN+1) != 0){
        return;
    }
    RCopyHeader hdr;
    memcpy(&hdr, PDU, HDR_LEN);
    std::cout << "+++++++++++++++++" << std::endl;
    std::cout <<  "Sequence Number: " << ntohl(hdr.sequenceNum) << std::endl;
    std::cout <<  "Flag: " << std::to_string(hdr.flag) << std::endl;
    std::cout <<  "Payload: " << payload << std::endl;
    std::cout <<  "Payload Len: " << payload.size()+1 << std::endl;
    std::cout << "+++++++++++++++++" << std::endl;

}




