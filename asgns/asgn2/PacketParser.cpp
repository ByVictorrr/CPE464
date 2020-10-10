#include "Parsers.hpp"

void LoginPacketParser::parse(uint8_t *pkt){
    uint16_t len = getPacketLen(pkt);
    uint8_t flag = pkt[2];
    uint8_t handLen = pkt[3];
    this->handName = std::string(pkt+3, pkt+3+handLen);
}
/* Client and Server both use */
void BroadcastPacketParser::parse(uint8_t *pkt){
    // (1 byte containing the length of sending client | send client)/ then message
    // step 1 - get the size
    uint16_t len = getPacketLen(pkt);
    uint8_t handLen = pkt[3];
    this->message = std::string(pkt+3, pkt+3+handLen);
    this->srcHandle = std::string(pkt+3+handLen, pkt+len);
}

void MulticastPacketParser::parse(uint8_t *pkt){
    // Step 1 - step 1 get src handle
    uint16_t pkt_len = getPacketLen(pkt);
    //uint8_t flag = 
    uint8_t srcHandleLen = pkt[3];
    this->srcHandle = std::string(pkt+3, (pkt+3)+srcHandleLen);
    // step 2 - get the message
    this->numHandles = pkt[srcHandle.size()+3];
    int start = srcHandle.size()+4;
    int i=start, numHan=0;
    while(i < pkt_len+start && numHan < numHandles){
        int hanLen = pkt[i];
        this->destHandles.push_back(std::string(pkt+i+1, pkt+i+1+hanLen));
        i+=(hanLen+1);
        numHan++;
    }
    // Got to the message (maybe check the )
    this->message = std::string((char*)(pkt+i));
}
