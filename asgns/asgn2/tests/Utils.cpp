
// Hugh Smith April 2017
// Network code to support TCP client server connections

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname6.h"
#include "Utils.hpp"
#include "Client.hpp"
#include "safe_sys_calls.h"







std::string PacketBuilder::formatBPacket(std::vector<std::string> &args, TCPClient &client){
    uint16_t msg_slot, pkt_len;
    uint8_t flag = BROADCAST;
    uint8_t *transBuff = client.transBuff;
    uint8_t hand_len = strlen(client.handle);
    // args = text
    // pkt = <hdr(3)|src-len(1)|src-name|text>
    std::string message = args[0];
    pkt_len = HDR_LEN+FLAG_LEN+1+hand_len+message.size()+1;
    msg_slot = 2+1+1-1;
    
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    memcpy(transBuff+3, &hand_len, 1);
    memcpy(transBuff+4, message.c_str(), message.size()+1);

    return "";
}


std::string PacketBuilder::formatLPacket(TCPClient &client){
    uint8_t *transBuff = client.transBuff;
    uint8_t flag = LIST_HANDLES;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN;
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    client.pkt_len = pkt_len;
    return "";
}

std::string PacketBuilder::formatEPacket(TCPClient &client){
    uint8_t *transBuff = client.transBuff;
    uint8_t flag = CLIENT_EXIT;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN;
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    client.pkt_len=pkt_len;
    return ""
}


// Assume input has no trailing or front whitespaces
/*
* Formats a chat-packet so that it has all necessary fields besides msg
* 
*@return where the message should be inserted in the packet
*/
std::string PacketBuilder::formatPacket(const std::string &input, TCPClient &client){
    std::string nextInput("");
    uint8_t flag;
    char cmd = input[1];
    std::string str_args = input.substr(2, input.size());
    std::vector<std::string> args = UserParser::parse(str_args);

    // Step 1 - parse to get all the fields then get size
    switch (std::toupper(cmd))
    {
    case 'M':
        nextInput = "M" +formatMPacket(args, client);
        break;
    case 'B':
        nextInput = "B"+  formatBPacket(args, client);
        break;
    case 'L':
        nextInput = "B" + formatLPacket(client);
        break;
    case 'E':
        nextInput = "B" + formatLPacket(client);
        break;
    default:
        return "";
        break;
    }
    return nextInput.size() == 1 ? "" : nextInput;
}


