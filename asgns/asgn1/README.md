# Program #1 – Packet Trace

## Objective
* This assignment will help you look at details such as layers of the protocol stack and what is in each header.

## Learning outcomes
1. Be able to develop network analysis software.
2. Be able to process and understand: 1. Ethernet Header, 2. ARP Header both Request and Reply, 3.
ICMP Header for Echo request and Echo Reply, 4. TCP Header, and 5. UDP Header.
3. Develop an understanding and ability to use some pcap functions as an application programming
interface (API) for capturing network traffic.
4. Be able to incorporate existing software where appropriate and allowed.
5. Be able to use Wireshark for network troubleshooting, analysis, software and communications
protocol development, and education. 1
6. Start to be able to use C in network programming.
7. Be able to better understand the protocol stack.


## Program 
Write a packet sniffer named "trace".
    trace: will use the pcap library to read a previously captured network trace and you will print out protocol header information for a number of header types (protocol).
### PCAP libary
* It allows for retrieval of packets from either a NIC or a pcap file. 
#### Examples on where the PCAP libary is used
* wireshark
    * need root privilege to capture of a nic 
        * Hence why we are using files

### Types of Headers that you need to support
* Eth 
* ARP (both reply and request)
* ICMP (both echo request and reply)
* TCP 
* UDP

##TODO : look at the pseduo header for tcp
### Integers
Endianness: all ints stored in a network packet are stored in network byte order (aka. big-endian)
Sizes: we need fixed width ints hence use stdint.h

### Internet Checksums and Pseudo Headers
* Computing IP checkums vs. TCP and UDP Checksums
    * Before computing the checksum over a TCP or UDP segment, the system prepends a pseduo header to the segment
    * Then computes the checksum over the header and segment data.
    * The lengith field is the length of the TCP or UDP segment without the psduedo header
#### Overlying data structures
* While using structs, c has the right to add arbitrary padding to struct, destroying alignmnet. 
    * Use packed structs


## Using Pcap lib

### Opening the device/pcap file for sniffing 

pcap_t *pcap_open_offline(const char *fname, char *errbuf): 
    * Desc: 
        * Creates a sniffing sesion
    * Args:
        * fname - the file to open  (pcap file format)
        * errbuf - is filled in with an appriopirate error message (this is assumed to hold at least PCAP_ERRBUF_SIZE)
    * Return:
        * a pcap_t * on sucess or NULL on failure( which then fills errbuf)



pcap_next_ex(pcap_t *p, struct pcap_pkthdr **pkt_header,const u_char **pkt_data): 
    * Desc:
        * reads the next packet and returns a sucess/failure
    * Args:
        * pkt_header - point to our packet header
        * pkt_data - point to our packet data
            * pkt_<header, data> are not to be freed by user
    * Return:
        * 1 if the packet was read without problems


## Using other functions
char *inet_ntoa(struct in_addr in):
    * Desc: converts the internet host address in, given in network byte order, to a string in IPv4 dotted notation
    (uses statically allocated buffer);

char *ether_ntoa(const struct ether_addr *addr):
    * same thing as inet_ntoa

#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);

  The  htonl()  function  converts  the  unsigned  integer  hostlong from host byte order to
       network byte order.

       The htons() function converts the unsigned short integer hostshort from host byte order to
       network byte order.

       The ntohl() function converts the unsigned integer netlong from network byte order to host
       byte order.

       The ntohs() function converts the unsigned short integer netshort from network byte  order
       to host byte order.
