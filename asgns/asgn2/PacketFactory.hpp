#ifndef PACKET_FACTORY_H
#define PACKET_FACTORY_H

#include "Utils.hpp"
#include "Parsers.hpp"
#include "Client.hpp"
class MCommandParser;
class BCommandParser;
class TCPClient;


// used by server plus + client
class PacketFactory{
    private:
    public:
        static std::map<char, flags_t>cmdToFlag;
        static uint8_t *buildMPacket(MCommandParser &cmd, TCPClient *client);
        static uint8_t *buildBPacket(BCommandParser &cmd, TCPClient *client);
        static uint16_t getPacketLen(uint8_t *pkt){
            uint16_t len;
            memcpy(&len, pkt, 2);
            return len;
        }
        static const std::map<char,flags_t> &getCmdToFlag(){return cmdToFlag;}

        
};
#endif