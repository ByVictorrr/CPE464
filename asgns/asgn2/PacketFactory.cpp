#include "PacketFactory.hpp"
#include <algorithm>

std::map<char, flags_t> PacketFactory::cmdToFlag = {
        {'M', MULTICAST},
        {'B', BROADCAST},
        {'L', LIST_HANDLES},
        {'E', CLIENT_EXIT}
};


/* Format <hdr len (2) | 1 byte flag | 1 byte src-han-len | src-han | dest-han1-len | dest-han1 | ... | msg */
uint16_t PacketFactory::buildMPacket(MCommandParser &cmd, TCPClient *client){
    //flags_t flag = PacketFactory::getCmdToFlag()[cmd.getCommand()];
    flags_t flag = cmdToFlag[cmd.getCommand()];
    uint8_t *transBuff = client->getBuff();
    char *srcHand = client->getHandle();
    uint8_t src_handle_len = std::strlen(srcHand);
    int cursor = 0;
    /** Get pkt_size first */
    uint16_t pkt_len = HDR_LEN+FLAG_LEN+1;
    uint16_t net_len;
    // Step 1 - src-handle length
    pkt_len+=(src_handle_len+1);
    // Step 2- - all destinations-handles length 
    for(size_t i = 0; i < cmd.getDestHandles().size(); i++){
        pkt_len+=(1+cmd.getDestHandles()[i].size());
    }
    pkt_len += cmd.getMessages().front().size()+1; // +1 for including the null char
    /** Done getting the pkt_size */

    /** Time to fill the trans buff */
    net_len = htons(pkt_len);
    memcpy(transBuff, &net_len, 2);
    cursor=2;
    memcpy(transBuff+cursor, &flag, 1);
    cursor++;
    transBuff[cursor++] = src_handle_len;
    memcpy(transBuff+cursor, srcHand, src_handle_len);
    cursor+=src_handle_len;
    memcpy(transBuff+cursor, &cmd.getNumHandles(), 1);
    cursor++;
    for(std::string &destHand: cmd.getDestHandles()){
        transBuff[cursor++] = destHand.size();
        memcpy(transBuff+cursor, destHand.c_str(), destHand.size());
        cursor+=destHand.size();
    }
    // set message
    memcpy(transBuff+cursor, cmd.getMessages().front().c_str(), cmd.getMessages().front().size());
    return pkt_len;
}

// format <hdr|1byte src-han-len | src-hand | message>
uint16_t PacketFactory::buildBPacket(BCommandParser &cmd, TCPClient *client){
    uint8_t flag = cmdToFlag[cmd.getCommand()];
    uint16_t pkt_len = 3, net_len;
    uint8_t *transBuff = client->getBuff();
    char *srcHand = client->getHandle();
    uint8_t srcHandLen = std::strlen(srcHand);
    int cursor = 0;
    pkt_len+=(1+srcHandLen)+cmd.getMessages().front().size()+1;
    net_len = htons(pkt_len);
    memcpy(transBuff, &net_len ,2);
    cursor+=2;
    transBuff[cursor++] = flag;
    transBuff[cursor++] = srcHandLen;
    memcpy(transBuff+cursor, srcHand ,srcHandLen);
    cursor+=srcHandLen;
    memcpy(transBuff+cursor, cmd.getMessages().front().c_str(), cmd.getMessages().front().size());
    return pkt_len;

}

uint16_t PacketFactory::buildLoginPacket(TCPClient *client){
    flags_t flag = CHECK_HANDLE;
    uint16_t pkt_len = 3, net_len;
    char *handle = client->getHandle();
    uint8_t *pkt  = client->getBuff();
    pkt_len+=(strlen(handle)+1);
    net_len=htons(pkt_len);
    memcpy(pkt, &net_len, 2);
    pkt[2] = flag;
    pkt[3] = strlen(handle);
    memcpy(pkt+4, handle, strlen(handle));
    return pkt_len; 
}