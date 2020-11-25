#include <iostream>
#include <utility>
#include "networks.hpp"
#include "cpe464.h"
#include "Packet.hpp"
#include "safe_sys_calls.h"
/*======================RCopyPacket Class============================*/

/*******RCopyHeader***************/

/* Constructor/Destructors */
RCopyHeader::RCopyHeader(uint32_t seqNum, uint16_t checksum, uint8_t flag)
: sequenceNum(seqNum), checksum(checksum), flag(flag){}
/* Settters */
void RCopyHeader::setSequenceNumber(uint32_t seqNum){ this->sequenceNum = seqNum;}
void RCopyHeader::setChecksum(uint16_t checksum){this->checksum = checksum;}
void RCopyHeader::setFlag(uint8_t flag){this->flag = flag;}
/* Getters */
/* Using ntohl because RCopyHeader is used to ship out */
uint32_t RCopyHeader::getSequenceNumber(){return this->sequenceNum;}
uint16_t RCopyHeader::getChecksum(){return this->checksum;}
uint8_t RCopyHeader::getFlag(){return this->flag;}
/* Utils */
int RCopyHeader::size()
{
    return sizeof(sequenceNum)+sizeof(checksum)+sizeof(flag);
}
void RCopyHeader::clear()
{ 
    this->sequenceNum = 0;
    this->checksum = 0;
    this->flag = 0;
}

/*******RCopyPacket***************/
/* Constructors */
RCopyPacket::RCopyPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadSize) 
:   header(seqNum, 0, flag), payloadSize(payloadSize)
{
    memset(this->payload, 0, MAX_PAYLOAD_LEN);
    if(payload != NULL)
        memcpy(this->payload, payload, payloadSize);
    this->header.setChecksum(computeChecksum(this->header, this->payload, payloadSize));
}

/* Getters */
RCopyHeader &RCopyPacket::getHeader(){return this->header;}
uint8_t *RCopyPacket::getPayload(){return this->payload;}
uint8_t *RCopyPacket::getRawPacket()
{
    static uint8_t rawPacket[MAX_PAYLOAD_LEN+HDR_LEN];
    uint32_t seqNum = htonl(this->header.getSequenceNumber());
    uint16_t cksum = this->header.getChecksum();
    uint8_t flag = this->header.getFlag();

    memset(rawPacket, 0, MAX_PAYLOAD_LEN+HDR_LEN);
    memcpy(rawPacket, &seqNum, sizeof(seqNum));
    memcpy(rawPacket+sizeof(seqNum), &cksum, sizeof(cksum));
    memcpy(rawPacket+sizeof(seqNum)+sizeof(cksum), &flag, sizeof(flag));
    memcpy(rawPacket+this->header.size(), payload, payloadSize);
    return rawPacket;
}
/* Utils */
int RCopyPacket::getPayloadSize(){return this->payloadSize;}
int RCopyPacket::size(){return this->payloadSize+this->header.size();}
void RCopyPacket::clear()
{
    memset(payload, 0, MAX_PAYLOAD_LEN);
    header.clear();
}

uint16_t RCopyPacket::computeChecksum(RCopyHeader &header, uint8_t *payload, int payloadSize)
{
    int cursor = 0;
    uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
    // Header vars
    uint32_t seqNum = htonl(this->header.getSequenceNumber());
    uint16_t cksum = 0;
    uint8_t flag = this->header.getFlag();
    memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
    // step 1 - packet
    memcpy(temp, &seqNum, sizeof(header.getSequenceNumber()));
    cursor+=sizeof(seqNum);
    memcpy(temp+cursor, &cksum, sizeof(header.getChecksum()));
    cursor+=sizeof(cksum);
    memcpy(temp+cursor, &flag, sizeof(header.getFlag()));
    cursor+=sizeof(flag);
    memcpy(temp+cursor, payload, payloadSize);
    // Step 2 - compute checksum
    cksum = in_cksum((unsigned short *)temp, payloadSize+header.size());
    return cksum;
}


/*******RCopySetupPacket***************/
RCopySetupPacket::RCopySetupPacket(uint32_t bufferSize, uint32_t windowSize, const char *fileName, uint8_t* payload)
: RCopyPacket(-1, FILENAME_PACKET, payload, sizeof(bufferSize)+sizeof(windowSize)+strlen(fileName)+1), 
  bufferSize(bufferSize), windowSize(windowSize), fileName(fileName){}

uint32_t RCopySetupPacket::getBufferSize(){return this->bufferSize;}
uint32_t RCopySetupPacket::getWindowSize(){return this->windowSize;}
std::string &RCopySetupPacket::getFileName(){return this->fileName;}


/*************RCopyACKPacket************/
RCopyACKPacket::RCopyACKPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload) 
: RCopyPacket(seqNum, flag, payload, sizeof(uint32_t)){}
/**************RCopyPacket Utility Functions******************/

RCopyPacket RCopyPacketBuilder::Build(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen)
{
    return RCopyPacket(seqNum, flag, payload, payloadLen);
}
RCopySetupPacket RCopyPacketBuilder::BuildSetup(uint32_t bufferSize, uint32_t windowSize, const char *fileName)
{
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint32_t rawBuffSize = htonl(bufferSize);
    uint32_t rawWindSize = htonl(windowSize);
    memset(payload, 0, MAX_PAYLOAD_LEN);
    memcpy(payload, &rawBuffSize, sizeof(bufferSize));
    memcpy(payload+sizeof(bufferSize), &rawWindSize, sizeof(windowSize));
    memcpy(payload+sizeof(bufferSize)+sizeof(windowSize), fileName, strlen(fileName)+1); // todo make var for max filename 100
    return RCopySetupPacket(bufferSize, windowSize, fileName, payload);
}


RCopyACKPacket RCopyPacketBuilder::BuildACK(uint32_t seqNum, uint8_t flag)
{
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint32_t rawSeqNum = htonl(seqNum);
    
    memset(payload, 0, MAX_PAYLOAD_LEN);
    memcpy(payload, &rawSeqNum, sizeof(seqNum));

    return RCopyACKPacket(seqNum, flag, payload);
}

ssize_t RCopyPacketSender::Send(RCopyPacket &packet, Connection &con)
{

    return safeSendToErr(con.getSocketNumber(),
                         packet.getRawPacket(), 
                         packet.size(), 
                         0, 
                         con.getRemote(),
                         *con.getRemoteLen()
                         );
}

ssize_t RCopyPacketSender::SendSetup(RCopySetupPacket &packet, Connection &con)
{
    return safeSendToErr(con.getSocketNumber(),
                         packet.getRawPacket(), 
                         packet.MAX_PAYLOAD_SIZE+HDR_LEN,
                         0, 
                         con.getRemote(),
                         *con.getRemoteLen()
                         );
}
ssize_t RCopyPacketSender::SendACK(RCopyACKPacket &packet, Connection &con)
{
    std::cout << "SENDACK payload size: " << packet.getPayloadSize() << std::endl;
    std::cout << std::to_string(packet.getHeader().getFlag()) << std::endl;
    return safeSendToErr(con.getSocketNumber(),
                         packet.getRawPacket(), 
                         packet.getPayloadSize()+HDR_LEN,
                         0, 
                         con.getRemote(),
                         *con.getRemoteLen()
                         );
}
/*===========================parser/recv========================================*/

RCopyPacket RCopyPacketParser::Parse(uint8_t *packet, int payloadLen)throw(CorruptPacketException)
{
    uint32_t seqNum;
    uint8_t flag;
    uint8_t *payload ;
    // step 1 - extract fields and tests checksum
    if(in_cksum((unsigned short *)packet, payloadLen+HDR_LEN)!=0){
        std::cout << "bad crc" << std::endl;
        throw CorruptPacketException("CRC error");
    }
    // step 2 - fill the RcopyPacket (assume correct)
    memcpy(&seqNum, packet, sizeof(seqNum));
    memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
    payload = packet + HDR_LEN;
    return RCopyPacket(ntohl(seqNum), flag, payload, payloadLen);
}

RCopySetupPacket RCopyPacketParser::ParseSetup(uint8_t *packet) throw(CorruptPacketException){
    uint32_t seqNum;
    uint8_t flag;
    uint8_t *payload;
    // setup vars
    char fileName[MAX_FILENAME_LEN];
    uint32_t bufferSize, windowSize;
    memset(fileName, 0, MAX_FILENAME_LEN);
    // step 1 - extract fields and tests checksum
    if(in_cksum((unsigned short *)packet, MAX_SETUP_PAYLOAD_LEN+HDR_LEN)!=0)
        throw CorruptPacketException("CRC error");
    // step 2 - fill the RcopyPacket (assume correct)
    // TODO : check if setup packet
    memcpy(&seqNum, packet, sizeof(seqNum));
    std::cout << "parsesetup: " << std::to_string(seqNum) << std::endl;
    memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
    payload = packet + HDR_LEN;
    memcpy(&bufferSize, payload, sizeof(bufferSize));
    memcpy(&windowSize, payload+sizeof(bufferSize), sizeof(windowSize));
    memcpy(&fileName, payload+sizeof(bufferSize)+sizeof(windowSize), MAX_FILENAME_LEN);
    //return RCopySetupPacket(ntohl(seqNum), flag, payload, payloadLen);
    return RCopySetupPacket(ntohl(bufferSize), ntohl(windowSize), fileName, payload);
}


RCopyACKPacket RCopyPacketParser::ParseACK(uint8_t *packet) throw(CorruptPacketException)
{
    uint32_t seqNum;
    uint8_t flag;
    uint8_t *payload = packet+HDR_LEN;
    memcpy(&seqNum, packet, sizeof(uint32_t));

    if(in_cksum((unsigned short *)packet, sizeof(uint32_t)+HDR_LEN)!=0)
        throw CorruptPacketException("CRC error");

    memcpy(&flag, packet+sizeof(uint32_t)+sizeof(uint16_t), sizeof(flag));
    std::cout << "parseACK: " << std::to_string(flag) << std::endl;
    //memcpy(&seqNum, payload, sizeof(uint32_t));
    return RCopyACKPacket(ntohl(seqNum), flag, payload);
}

RCopyPacket RCopyPacketReciever::Recieve(int payloadLen, Connection &con) throw(CorruptPacketException){
    static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
    int recvLen;
    memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);

    // Step 1 - recv packet
    recvLen = safeRecvfrom(con.getSocketNumber(), temp, payloadLen+HDR_LEN, 0,con.getRemote(), con.getRemoteLen());
    // Step 2 - parse packet
    try{
        return RCopyPacketParser::Parse(temp, recvLen-HDR_LEN);
    }catch(CorruptPacketException &e){
        throw e;
    }
}
RCopySetupPacket RCopyPacketReciever::RecieveSetup(Connection &con) throw(CorruptPacketException){
    static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
    memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
    // Step 1 - recv packet
    safeRecvfrom(con.getSocketNumber(), temp, RCopySetupPacket::MAX_PAYLOAD_SIZE+HDR_LEN, 0,con.getRemote(), con.getRemoteLen());
    // Step 2 - parse packet
    try{
        return RCopyPacketParser::ParseSetup(temp);
    }catch(CorruptPacketException &e){
        throw e;
    }
}


RCopyACKPacket RCopyPacketReciever::RecieveACK(Connection &con) throw(CorruptPacketException)
{
    static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
    memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
    // Step 1 - recv packet
    int len = safeRecvfrom(con.getSocketNumber(), temp, RCopyACKPacket::MAX_PAYLOAD_SIZE+HDR_LEN, 0,con.getRemote(), con.getRemoteLen());
    // Step 2 - parse packet
    try{
        return RCopyPacketParser::ParseACK(temp);
    }catch(CorruptPacketException &e){
        throw e;
    }
}



