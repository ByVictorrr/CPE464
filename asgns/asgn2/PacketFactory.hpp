#include "Utils.hpp"
#include "CommandParser.hpp"

// used by server plus + client
class PacketFactory{
        std::map<char, flags_t>cmdToFlag = {
            {'M', MULTICAST},
            {'B', BROADCAST},
            {'L', LIST_HANDLES},
            {'E', CLIENT_EXIT}
        };
    public:
        /* Format <hdr len (2) | 1 byte flag | 1 byte src-han-len | src-han | dest-han1-len | dest-han1 | ... | msg */
        static uint8_t *buildMPacket(MCommandParser &cmd, TCPClient *client){
            uint8_t *transBuff = client->getTransBuff();
            char *srcHand = client->getHandle();
            uint8_t src_handle_len = std::strlen(srcHand);
            int cursor = 0;
            memset(transBuff,0 ,MAX_BUFF);
            /** Get pkt_size first */
            uint16_t pkt_len = HDR_LEN+FLAG_LEN+1;
            // Step 1 - src-handle length
            pkt_len+=(src_handle_len+1);
            // Step 2- - all destinations-handles length 
            for(int i = 0; i < cmd.getDestHandles().size(); i++){
                pkt_len+=(1+cmd.getDestHandles()[i].size());
            }
            pkt_len += cmd.getMessages().front().size()+1; // +1 for including the null char
            /** Done getting the pkt_size */

            /** Time to fill the trans buff */
            memcpy(transBuff, &pkt_len, 2);
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
            return transBuff;
        }
        
};