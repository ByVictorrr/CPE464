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
       virtual u_char *set_fields(const u_char *pkt_data) = 0;
};

class EthHDR: public HDR{
    private:
        std::string dest;
        std::string src;
        std::string type;

        void findType(uint8_t raw_type[2]){
            if(raw_type[0] == 0x08 && raw_type[1] == 0x00){
                type="IP";
            }else{
                type="ARP";
            }
        }
    public:        

        static const int LENGTH = 14;
        /**
         * @par: pkt_data the raw pkt data
         * @return: a ptr to pkt_data that we are reading
         */
        u_char *set_fields(const u_char *pkt_data){
            struct ether_addr dest_ether, src_ether;
            uint8_t raw_type[3];
            memcpy(dest_ether.ether_addr_octet, pkt_data, ETHER_ADDR_LEN);
            memcpy(src_ether.ether_addr_octet, pkt_data+ETHER_ADDR_LEN, ETHER_ADDR_LEN);
            memcpy(raw_type, pkt_data+2*ETHER_ADDR_LEN, 2);

            this->src = ether_ntoa(&src_ether);
            this->dest = ether_ntoa(&dest_ether);
            findType(raw_type);
            return (u_char*)pkt_data + 2*ETHER_ADDR_LEN + 2;
        }

        std::string getType(){
            return type;
        }
        friend std::ostream &operator<<(std::ostream &out, const EthHDR & hdr){
            out << "\tEthernet Header" << std::endl;
            out << "\t\tDest MAC: " << hdr.dest << std::endl;
            out << "\t\tSource MAC: " << hdr.src << std::endl;
            out << "\t\tType: " << hdr.type << std::endl;
            return out;

        }

};
/*
class ArpHDR: public HDR{
    private:
        static const int LENGTH = 28;
        char request[10];
        char senderMAC, senderIP;
        char destMAC, destIP;
    public:
        u_char *set_fields(u_char *pkt_data){
            
            memccpy()  
            memcpy(dest_ether.ether_addr_octet, (void*)pkt_data, 21);
            memcpy(src_ether.ether_addr_octet, (void*)pkt_data+ETHER_ADDR_LEN, ETHER_ADDR_LEN);
            memcpy(raw_type, (void*)pkt_data+2*ETHER_ADDR_LEN, 2);
            this->src = ether_ntoa(&src_ether);
            this->dest = ether_ntoa(&dest_ether);

        }
};
*/


void print_HDR(HDR *hdr){

}


int main(int argc, char *argv[]){
    pcap_t *pcap;
    struct pcap_pkthdr *pkt_hdr;
    const u_char *pkt_data;
    
    // usage
    if(argc != 2){
        std::cerr << "usage: trace <file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // else do the program
    if ((pcap = pcap_open_offline(argv[1], errbuf)) == NULL){
        std::cerr << errbuf << std::endl;
        exit(EXIT_FAILURE);
    }
    

    // Keep reading till no more packets are to be read
    // for every packet in our pcap file
    int pkt_num = 1;
    std::cout << std::endl;
    while(pcap_next_ex(pcap, &pkt_hdr, &pkt_data) != PCAP_ERROR_BREAK){
        // go through each field of the packet
        u_char *curr_spot = (u_char*)pkt_data;
        EthHDR eth;
        curr_spot = eth.set_fields(curr_spot);
        // check at the end pkt_data - curr_spot == pkt_hdr->len

        std::cout << "Packet number: " << pkt_num++ << " Frame Len: " << pkt_hdr->len 
                  << std::endl << std::endl;

        std::cout << eth << std::endl;
        /*
        if(strcmp(eth.getType(), "ARP") == 0){
        // ipv4
        }else{
            //IpHDR ip;
            ;
        }
        */

    }

    return 0;
}