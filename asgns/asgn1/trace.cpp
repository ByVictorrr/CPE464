#include <pcap/pcap.h>
#include <iostream>
#include <ostream>
#include <cstdlib>
#include <string.h>
#include <netinet/ether.h>

char errbuf[PCAP_ERRBUF_SIZE];

// type = {arp, ipv, unknown}

// Interface HDR
class HDR{
    public:
        // abstract method
       virtual const u_char* set_fields(const u_char *pkt_hdr) = 0;
};

class EthHDR: public HDR{
    private:
        std::string dest;
        std::string src;
        std::string type;
        static const uint16_t ARP_TYPE = 0x0806;
        static const uint16_t IPV4_TYPE = 0x0800;

        void findType(uint16_t raw_type){
            uint16_t type = ntohs(raw_type);
            if(type == ARP_TYPE){
                this->type = "ARP";
            }else if(type == IPV4_TYPE){
                this->type = "IP";
            }else{
                this->type = "DNE";
            }
        }
    public:        


        /**
         * @par: pkt_data the raw pkt hdr
         * @return: A cursor that points to the payload
         */
        const u_char *set_fields(const u_char *pkt_hdr){
            struct ether_addr dest_ether, src_ether;
            uint16_t raw_type;
            memcpy(dest_ether.ether_addr_octet, pkt_hdr, ETHER_ADDR_LEN);
            memcpy(src_ether.ether_addr_octet, pkt_hdr+ETHER_ADDR_LEN, ETHER_ADDR_LEN);
            memcpy(&raw_type, pkt_hdr+2*ETHER_ADDR_LEN, 2);

            // ether_ntoa 
            this->dest = ether_ntoa(&dest_ether);
            this->src = ether_ntoa(&src_ether);
            findType(raw_type);
            return pkt_hdr+2*ETHER_ADDR_LEN + 2;
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
        std::string destMAC, destIP;
    public:
        const u_char *set_fields(const u_char *pkt_hdr){
            int offset = 6; // offset to first relevant field in the hdr
            #define IP_ADDR_LEN 4
            struct ether_addr target_ether, sender_ether;
            struct in_addr target_ip, sender_ip;
            memcpy(sender_ether.ether_addr_octet, pkt_hdr + offset, ETHER_ADDR_LEN);
            memcpy(&sender_ip.s_addr, pkt_hdr + offset+ETHER_ADDR_LEN, IP_ADDR_LEN);
        
            memcpy(dest_ether.ether_addr_octet, pkt_hdr+offset, 21);
            memcpy(raw_type, (void*)pkt_data+2*ETHER_ADDR_LEN, 2);
            this->src = ether_ntoa(&src_ether);
            this->dest = ether_ntoa(&dest_ether);

        }
};




int main(int argc, char *argv[]){
    pcap_t *pcap_sess;
    int next;
    struct pcap_pkthdr *pkt_hdr;
    const u_char *pkt_data;
    int cursor;
    
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

        eth.set_fields(pkt_data);

        // check at the end pkt_data - curr_spot == pkt_hdr->len

        std::cout << "Packet number: " << pkt_num++ << " Frame Len: " << pkt_hdr->len 
                  << std::endl << std::endl;

        std::cout << eth << std::endl;

        if(eth.getType().compare("ARP") == 0){
            
        }else if (eth.getType().compare("IP") == 0){

        }

    }

    return 0;
}