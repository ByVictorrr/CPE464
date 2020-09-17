#include <pcap/pcap.h>
#include <iostream>
#include <ostream>
#include <cstdlib>
#include <string.h>
#include <netinet/ether.h>

char errbuf[PCAP_ERRBUF_SIZE];
#define IP_ADDR_LEN 4

struct eth_hdr{
    uint8_t dest[ETHER_ADDR_LEN];
    uint8_t src[ETHER_ADDR_LEN];
    uint16_t type;
}__attribute__((packed));

struct arp_hdr{
    uint16_t hw_type;
    uint16_t prot_type;
    uint8_t hw_size;
    uint8_t prot_size;
    uint16_t opcode;
    uint8_t sender_mac[ETHER_ADDR_LEN];
    uint8_t sender_ip[IP_ADDR_LEN];
    uint8_t target_mac[ETHER_ADDR_LEN];
    uint8_t target_ip[IP_ADDR_LEN];

}__attribute__((packed));

// Interface HDR
class HDR{
    public:
        // abstract method
       virtual const u_char* set_fields(const u_char *pkt_hdr) = 0;
};
class EthHDR : public HDR{
private:
        std::string dest;
        std::string src;
        std::string type;
        static const uint16_t ARP_TYPE = 0x0806;
        static const uint16_t IPV4_TYPE = 0x0800;

        std::string getType(uint16_t raw_type){
            uint16_t type = ntohs(raw_type);
            if(type == ARP_TYPE){
                return "ARP";
            }else if(type == IPV4_TYPE){
                return "IP";
            }else{
                return "DNE";
            }
        }
    public:        


        /**
         * @par: pkt_data the raw pkt hdr
         * @return: A cursor that points to the payload
         */
        const u_char *set_fields(const u_char *pkt_hdr){
            struct eth_hdr *hdr = (struct eth_hdr*)pkt_hdr;
            this->dest = ether_ntoa((const ether_addr *)hdr->dest);
            this->src = ether_ntoa((const ether_addr *)hdr->src);
            this->type = getType(hdr->type);
            return pkt_hdr+sizeof(struct eth_hdr);
        }
        inline std::string getType(){
            return this->type;
        }

        friend std::ostream &operator<<(std::ostream &out, const EthHDR & hdr){
            out << "\tEthernet Header" << std::endl;
            out << "\t\tDest MAC: " << hdr.dest << std::endl;
            out << "\t\tSource MAC: " << hdr.src << std::endl;
            out << "\t\tType: " << hdr.type << std::endl;
            return out;

        }

};


class ArpHDR: public HDR{
    private:
        // static const int LENGTH = 28;
        std::string opcode;
        std::string senderMAC, senderIP;
        std::string targetMAC, targetIP;
        static const uint16_t REQUEST_OPCODE = 1; 
        static const uint16_t REPLY_OPCODE = 2;

        std::string getOpcode(uint16_t raw_opcode){
            uint16_t opcode = ntohs(raw_opcode);
            if(opcode == REQUEST_OPCODE){
                return "Request";
            }else if (opcode == REPLY_OPCODE){
                return "Reply";
            }else{
                return "DNE";
            }
        }
    public:
        const u_char *set_fields(const u_char *pkt_hdr){
            struct arp_hdr *hdr = (struct arp_hdr*)pkt_hdr;
            struct in_addr sender_ip, target_ip;
            memcpy(&sender_ip, &(hdr->sender_ip), sizeof(sender_ip));
            memcpy(&target_ip, &(hdr->target_ip), sizeof(target_ip));
            this->opcode = getOpcode(hdr->opcode);
            this->senderMAC = ether_ntoa((const ether_addr *)hdr->sender_mac);
            this->targetMAC = ether_ntoa((const ether_addr *)hdr->target_mac);
            this->senderIP = inet_ntoa(sender_ip);
            this->targetIP = inet_ntoa(target_ip);
            return pkt_hdr + sizeof(struct arp_hdr);
        }
        friend std::ostream &operator<<(std::ostream &out, const ArpHDR & hdr){
            out << "\tARP Header" << std::endl;

            out << "\t\tOpcode: " << hdr.opcode << std::endl;
            out << "\t\tSender MAC: " << hdr.senderMAC << std::endl;
            out << "\t\tSender IP: " << hdr.senderIP << std::endl;
            out << "\t\tTarget MAC: " << hdr.targetMAC << std::endl;
            out << "\t\tTarget IP: " << hdr.targetIP << std::endl;
            return out;

        }
};




int main(int argc, char *argv[]){
    pcap_t *pcap_sess;
    int next;
    struct pcap_pkthdr *pkt_hdr;
    const u_char *pkt_data, *cursor;
    
    // Step 1 - check usage
    if(argc != 2){
        std::cerr << "usage: trace <file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Step 2 - open file
    if ((pcap_sess = pcap_open_offline(argv[1], errbuf)) == NULL){
        std::cerr << errbuf << std::endl;
        exit(EXIT_FAILURE);
    }
    

    // Keep reading till no more packets are to be read
    // for every packet in our pcap file
    int pkt_num = 1;
    std::cout << std::endl;
    EthHDR eth;
    ArpHDR arp;
    while((next=pcap_next_ex(pcap_sess, &pkt_hdr, &pkt_data)) != PCAP_ERROR_BREAK || next != PCAP_ERROR){

        cursor = eth.set_fields(pkt_data);

        // check at the end pkt_data - curr_spot == pkt_hdr->len

        std::cout << "Packet number: " << pkt_num++ << " Frame Len: " << pkt_hdr->len 
                  << std::endl << std::endl;

        std::cout << eth << std::endl;

        if(eth.getType().compare("ARP") == 0){
            arp.set_fields(cursor);
            std::cout << arp << std::endl;
        }else if (eth.getType().compare("IP") == 0){

        }

    }

    return 0;
}