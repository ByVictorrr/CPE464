#include <pcap/pcap.h>
#include <iostream>
#include <ostream>
#include <cstdlib>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include "checksum.h"

char errbuf[PCAP_ERRBUF_SIZE];
#define IP_ADDR_LEN 4

struct eth_hdr{
    uint8_t dest[ETHER_ADDR_LEN];
    uint8_t src[ETHER_ADDR_LEN];
    uint16_t upper_layer;
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


struct ip_hdr{
    uint8_t ip_ver_hdrlen; // bit 0-4 are ver and 4-8 are hdr len
    uint8_t tos; // type of service like audio, ect.
    uint16_t datagram_len; // hdr+payload len in bytes
    uint16_t id; // identifys for fragmentation and resemble or related packets
    uint16_t flags_fragoff; // flags for fragmentation
    uint8_t time_live; // number of hops
    uint8_t upper_layer; // higher level protocol in payload
    uint16_t hdr_chksum; // checksum
    uint32_t source_ip;
    uint32_t dest_ip;
}__attribute__((packed));

struct pseduo_hdr{
    uint32_t src_ip;
    uint32_t dest_ip;
    uint8_t reserved;
    uint8_t protocol;
    uint16_t protocol_len;
}__attribute__((packed));

struct tcp_hdr{
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t hdr_len_res_flags; // 4 bits hdr len and 8 reserved (0's) and 6 flags (6 bits)
    uint16_t advertise_window;
    uint16_t checksum;
    uint16_t urgent_ptr;
}__attribute__((packed));

struct udp_hdr{
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t pdu_len;
    uint16_t check_sum;
}__attribute__((packed));

struct icmp_hdr{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_num;
}__attribute__((packed));



// Interface HDR
class HDR{
    public:
       virtual inline const u_char* getPayload() = 0;
};

class EthHDR : public HDR{
private:
        struct eth_hdr *hdr;
        static const uint16_t ARP_TYPE = 0x0806;
        static const uint16_t IPV4_TYPE = 0x0800;

        std::string getUpperLayer(uint16_t upperLayer){
            if(upperLayer == ARP_TYPE){
                return "ARP";
            }else if(upperLayer == IPV4_TYPE){
                return "IP";
            }else{
                return "DNE";
            }
        }
    public:        

        EthHDR(const u_char *pkt_hdr){
            this->hdr = (struct eth_hdr*)pkt_hdr;
        }

        inline const u_char *getPayload(){return ((const u_char *)this->hdr)+sizeof(struct eth_hdr);}
        inline std::string getUpperLayer(){return this->getUpperLayer(ntohs(this->hdr->upper_layer));}

        friend std::ostream &operator<<(std::ostream &out, EthHDR & eth){
            struct eth_hdr *hdr = eth.hdr;
            out << "\tEthernet Header" << std::endl;
            out << "\t\tDest MAC: " << ether_ntoa((const ether_addr *)hdr->dest) << std::endl;
            out << "\t\tSource MAC: " << ether_ntoa((const ether_addr *)hdr->src) << std::endl;
            out << "\t\tType: " << eth.getUpperLayer() << std::endl;
            return out;

        }

};


class ArpHDR: public HDR{
    private:
        struct arp_hdr *hdr;
        static const uint16_t REQUEST_OPCODE = 1; 
        static const uint16_t REPLY_OPCODE = 2;

        std::string getOpcode(uint16_t opcode){
            if(opcode == REQUEST_OPCODE){
                return "Request";
            }else if (opcode == REPLY_OPCODE){
                return "Reply";
            }else{
                return "DNE";
            }
        }
    public:
        ArpHDR(const u_char *pkt_hdr){
            this->hdr = (struct arp_hdr*)pkt_hdr;
        }
        inline const u_char *getPayload(){return (const u_char *)this->hdr + sizeof(struct arp_hdr);}
        inline std::string getOpcode(){return this->getOpcode(ntohs(this->hdr->opcode));}
        friend std::ostream &operator<<(std::ostream &out, ArpHDR & arp){
            struct in_addr sender_ip, target_ip;
            struct arp_hdr *hdr = arp.hdr;

            memcpy(&sender_ip, &(arp.hdr->sender_ip), sizeof(sender_ip));
            memcpy(&target_ip, &(arp.hdr->target_ip), sizeof(target_ip));
            
            out << "\tARP header" << std::endl;
            out << "\t\tOpcode: " << arp.getOpcode() << std::endl;
            out << "\t\tSender MAC: " << ether_ntoa((const ether_addr *)hdr->sender_mac) << std::endl;
            out << "\t\tSender IP: " << inet_ntoa(sender_ip) << std::endl;
            out << "\t\tTarget MAC: " << ether_ntoa((const struct ether_addr *)hdr->target_mac) << std::endl;
            out << "\t\tTarget IP: " << inet_ntoa(target_ip) << std::endl;
            return out;

        }
};
class IpHDR: public HDR{
    private:
        struct ip_hdr *hdr;
        struct pseduo_hdr *p_hdr;
        static const uint8_t TCP_TYPE = 6;
        static const uint8_t UDP_TYPE = 17;
        static const uint8_t ICMP_TYPE = 1;

        std::string getUpperLayer(uint8_t upperLayer){
            if(upperLayer == TCP_TYPE){
                return "TCP";
            }else if (upperLayer == UDP_TYPE){
                return "UDP";
            }else if(upperLayer == ICMP_TYPE){
                return "ICMP";
            }else{
                return "Unknown";
            }
        }
       struct pseduo_hdr *_getPseduoHDR(){
            struct pseduo_hdr *p_hdr;
            struct ip_hdr * hdr = this->hdr;
            if(!(p_hdr=(struct pseduo_hdr *)malloc(sizeof(struct pseduo_hdr)))){
                return NULL;
            }
            p_hdr->dest_ip = hdr->dest_ip;
            p_hdr->src_ip = hdr->source_ip;
            p_hdr->reserved = 0;
            p_hdr->protocol = hdr->upper_layer;
            p_hdr->protocol_len = htons(ntohs(hdr->datagram_len) - ((hdr->ip_ver_hdrlen & 0x0f)*4));
            return p_hdr;
        }

    public:
        IpHDR(const u_char *pkt_hdr){
            this->hdr = (struct ip_hdr*)pkt_hdr;
            this->p_hdr = NULL;
        }
        inline const u_char *getPayload(){return (const u_char*)this->hdr + ((this->hdr->ip_ver_hdrlen & 0x0f)*4);}
        inline std::string getUpperLayer(){return this->getUpperLayer(this->hdr->upper_layer);}
        inline struct ip_hdr *getHDR(){return this->hdr;}
        inline struct pseduo_hdr *getPseduoHDR(){
            if(this->p_hdr == NULL){
                this->p_hdr = this->_getPseduoHDR();
            }
            return this->p_hdr;
        }

        friend std::ostream &operator<<(std::ostream &out, IpHDR & ip){
            struct in_addr sender_ip, dest_ip;
            int hdr_len = (ip.hdr->ip_ver_hdrlen & 0x0f)*4;
            memcpy(&sender_ip, &(ip.hdr->source_ip), sizeof(sender_ip));
            memcpy(&dest_ip, &(ip.hdr->dest_ip), sizeof(dest_ip));
            
            out << "\tIP Header" << std::endl;
            out << "\t\tHeader Len: " << hdr_len  << " (bytes)" << std::endl;
            out << "\t\tTOS: 0x" << std::hex << ((int)ip.hdr->tos) << std::endl;
            out << "\t\tTTL: " << std::dec << (int)ip.hdr->time_live << std::endl;
            out << "\t\tIP PDU Len: " << std::dec << ntohs(ip.hdr->datagram_len)  << " (bytes)" << std::endl;
            out << "\t\tProtocol: " << ip.getUpperLayer(ip.hdr->upper_layer) << std::endl;
            if(in_cksum((unsigned short *)ip.hdr, hdr_len) == 0){
              out << "\t\tChecksum: " << "Correct" << " (0x" << std::hex << std::noshowbase << ip.hdr->hdr_chksum << ")" << std::endl;
            }else{
              out << "\t\tChecksum: " << "Incorrect" << " (0x" << std::hex << std::noshowbase << ip.hdr->hdr_chksum << ")" << std::endl;
            }
            out << "\t\tSender IP: " << inet_ntoa(sender_ip) << std::endl;
            out << "\t\tDest IP: " << inet_ntoa(dest_ip) << std::endl;

            return out;

        }
        };

class TcpHDR : public HDR{
    private:
        struct tcp_hdr *tcp_hdr;
        struct pseduo_hdr *p_hdr;
        static const uint16_t HTTP = 80;
        
 
    public:
        TcpHDR(const u_char *pkt_hdr, struct pseduo_hdr *p_hdr){
            this->tcp_hdr = (struct tcp_hdr *)pkt_hdr;
            this->p_hdr = p_hdr;
        }
        inline const u_char *getPayload(){
            return (const u_char *)this->tcp_hdr + sizeof(struct tcp_hdr);
        }
    friend std::ostream &operator<<(std::ostream &out, const TcpHDR & tcp){
            u_char *combined_hdr;
            struct pseduo_hdr *p_hdr = tcp.p_hdr;
            struct tcp_hdr *tcp_hdr = tcp.tcp_hdr;
            int ack_valid = ntohs(tcp_hdr->hdr_len_res_flags) & 0b010000 ;

            out << "\tTCP Header" << std::endl;
            out << "\t\tSource Port: " << (ntohs(tcp_hdr->src_port) == tcp.HTTP ? "HTTP" : std::string(": ") + std::to_string(ntohs(tcp_hdr->src_port))) << std::endl;
            out << "\t\tDest Port: " << (ntohs(tcp_hdr->dest_port) == tcp.HTTP ? "HTTP" : std::string(": ")+std::to_string(ntohs(tcp_hdr->dest_port))) << std::endl;
            out << "\t\tSequence Number: " << std::dec << ntohl(tcp_hdr->seq_num) << std::endl;
            out << "\t\tACK Number: " << (ack_valid ? std::to_string(ntohl(tcp_hdr->ack_num)) : "<not valid>") << std::endl;
            out << "\t\tACK Flag: " << (ack_valid ? "Yes" : "No") << std::endl;
            out << "\t\tSYN Flag: " << (ntohs(tcp_hdr->hdr_len_res_flags) & 0b000010 ? "Yes" : "No") << std::endl;
            out << "\t\tRST Flag: " << (ntohs(tcp_hdr->hdr_len_res_flags) & 0b000100 ? "Yes" : "No") << std::endl;
            out << "\t\tFIN Flag: " << (ntohs(tcp_hdr->hdr_len_res_flags) & 0b000001 ? "Yes" : "No") << std::endl;
            out << "\t\tWindow Size: " << std::dec << ntohs(tcp_hdr->advertise_window) << std::endl;
            if(p_hdr == NULL)
                exit(EXIT_FAILURE);

            if(!(combined_hdr = (u_char*)malloc(sizeof(struct pseduo_hdr)+ntohs(p_hdr->protocol_len)+2)))
                exit(EXIT_FAILURE);

            memcpy(combined_hdr, p_hdr, sizeof(struct pseduo_hdr));
            memcpy(combined_hdr+sizeof(struct pseduo_hdr), tcp_hdr, ntohs(p_hdr->protocol_len));


            if(in_cksum((unsigned short *)combined_hdr, sizeof(struct pseduo_hdr) + ntohs(p_hdr->protocol_len)) == 0){
              out << "\t\tChecksum: " << "Correct" << " (" << std::hex << std::showbase << ntohs(tcp_hdr->checksum) << ")" << std::endl;
            }else{
              out << "\t\tChecksum: " << "Incorrect" << " (" << std::hex << std::showbase << ntohs(tcp_hdr->checksum) << ")" << std::endl;
            }

            free(combined_hdr);
            free(p_hdr);
            return out;

        }


};

class UdpHDR : public HDR{
    private:
        struct udp_hdr *hdr;
 
    public:
        UdpHDR(const u_char *pkt_hdr){
            this->hdr = (struct udp_hdr *)pkt_hdr;
        }
        inline const u_char *getPayload(){
            return (const u_char *)this->hdr + sizeof(struct udp_hdr);
        }
        friend std::ostream &operator<<(std::ostream &out, const UdpHDR & udp){
                struct udp_hdr *hdr = udp.hdr;
                out << "\tUDP Header" << std::endl;
                out << "\t\tSource Port: : " << std::dec << ntohs(hdr->src_port) << std::endl;
                out << "\t\tDest Port: : " << std::dec << ntohs(hdr->dest_port) << std::endl;
                return out;
        }

};

class IcmpHDR : public HDR{
    private:
        struct icmp_hdr *hdr;
        static const uint8_t REQUEST_TYPE = 8;
        static const uint8_t REPLY_TYPE = 0;
        std::string getType(uint8_t type) const{
            if(type == REQUEST_TYPE){
                return "Request";
            }else if(type == REPLY_TYPE){
                return "Reply";
            }
            return std::to_string(type);
        }
 
    public:
        IcmpHDR(const u_char *pkt_hdr){
            this->hdr = (struct icmp_hdr *)pkt_hdr;
        }
        inline const u_char *getPayload(){
            return (const u_char *)this->hdr + sizeof(struct icmp_hdr);
        }
        friend std::ostream &operator<<(std::ostream &out, const IcmpHDR & icmp){
                struct icmp_hdr *hdr = icmp.hdr;
                out << "\tICMP Header" << std::endl;
                out << "\t\tType: " << icmp.getType(hdr->type) << std::endl;

                return out;

        }

};





// After setting TCPHDR


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

    while((next=pcap_next_ex(pcap_sess, &pkt_hdr, &pkt_data)) == 1){

        cursor = pkt_data;
        EthHDR eth(cursor);
        cursor = eth.getPayload();


        std::cout << "Packet number: " << std::dec << pkt_num++ << " Frame Len: " << pkt_hdr->len 
                  << std::endl << std::endl;

        std::cout << eth << std::endl;

        if(eth.getUpperLayer().compare("ARP") == 0){
            ArpHDR arp(cursor);
            std::cout << arp << std::endl;
        }else if (eth.getUpperLayer().compare("IP") == 0){
            
            IpHDR ip(cursor);
            std::cout << ip << std::endl;
            cursor = ip.getPayload();
            // Case where protocol is TCP
            if(ip.getUpperLayer().compare("TCP") == 0){

                TcpHDR tcp(cursor, ip.getPseduoHDR());
                std::cout << tcp << std::endl;

            }else if(ip.getUpperLayer().compare("UDP") == 0){
                UdpHDR udp(cursor);
                std::cout << udp << std::endl;

            }else if(ip.getUpperLayer().compare("ICMP") == 0){
                IcmpHDR icmp(cursor);
                std::cout << icmp << std::endl;
            }
        }

    }
    pcap_close(pcap_sess);

    return 0;
}
