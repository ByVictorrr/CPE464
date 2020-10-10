#include "Client.hpp"
#include <numeric>
#include <queue>
#include "Utils.hpp"
#include "CommandParser.hpp"
#include "PacketParser.hpp"

/***************** Client functions ****************************/
/**
 * Requirements:
 *  0.) No more than 1400 bytes including everything
 *  1.) msg must be at most 200 bytes in length
 *  2.) If msg is more than 200 bytes make sure to break it up into multiple packets
 *  3.) if no msg is given include just a new line as the message
 */



Client::Client(char *handle, char *server_name, char *port, int type, int protocol)  
: handle(handle), serverName(server_name), port(port)
{
    this->skt = safe_socket(AF_INET6, type, protocol);
    memset(recvBuff, 0, MAX_BUFF);
    memset(transBuff, 0, MAX_BUFF);
}
Client::~Client(){
    safe_close(this->skt);
}


TCPClient::TCPClient(char *handle, char *server_name, char *port, int protocol)  
: Client(handle, server_name, port, SOCK_STREAM, protocol){
}

void TCPClient::connect(int debugFlag){
    uint8_t * ipAddress = NULL;
    struct sockaddr_in6 server; 
    if(debugFlag)
        printf("Connecting to server on port number %s\n", port);

    // setup the server structure
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(atoi(this->port));

    // get the address of the server 
    if ((ipAddress = getIPAddress6(serverName, &server)) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    printf("server ip address: %s\n", getIPAddressString(ipAddress));
    safe_connect(this->skt, (struct sockaddr*)&server, sizeof(server));

    // TODO: create packet check to see if handle is taken
    safe_send(this->skt, this->handle, strlen(this->handle)+1,0);

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString(ipAddress), atoi(this->port));
    }
}

void TCPClient::processUserInput(){
    std::string input;
    uint16_t pkt_len;
    std::vector<std::string> parsedInput;
    std::queue<std::string> messages;
    try{
        // Step 1 - read and validate input
        std::getline(std::cin, input);
        if(CommandValidator::validate(input)){
            // Step 2 - see if it has a message
            switch (CommandParser::getCommand(input))
            {
            case 'M':
                {
                    MCommandParser parser;
                    parser.parse(input);
                    while(!parser.getMessages().empty()){
                        // this below could possibly be a static function class (returns uin8t_t)
                        uint8_t *pkt = PacketFactory::buildMPacket(parser, this);
                        safe_send(this->skt, pkt, PacketFactory::getPacketLen(pkt), 0);
                        parser.getMessages().pop();
                    }
                }
                break;
            case 'B':
                
                break;
            case 'L':
                break;
            case 'E':
                break;
            
            }

        }
    }catch(const char *msg){
        std::cerr << msg << std::endl;
    }
}

void TCPClient::processSocket(){
    uint16_t pkt_len;
    uint8_t flag;
    uint8_t *data;
    memset(recvBuff, 0, MAX_BUFF);
    pkt_len = read_pkt(this->skt, this->recvBuff);
    switch((flag=recvBuff[2]))
    {

        case HANDLE_DNE:
            std::cout << "Login sucessful" << std::endl;
        break;
        case HANDLE_EXISTS:
            safe_close(this->skt);
            exit(EXIT_FAILURE);
        break;
        case BROADCAST:
        {
            BroadcastPacketParser parser;
            parser.parse(recvBuff);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage();
        }
        case MULTICAST:
        {
            MulticastPacketParser parser;
            parser.parse(recvBuff);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage() << std::endl;
        }
        case MULTICAST_HANDLE_DNE:
            // make a parser (and throw error )
        break;
        case CLIENT_EXIT_ACK:
        {
            safe_close(this->skt);
            exit(EXIT_SUCCESS);
        }
        break;
        case NUM_HANDLES:
        {
            int numHandles;
            // print out the number
            memcpy(&numHandles, &recvBuff[3], 4);
            std::cout << "Number of handles: " << numHandles << std::endl;
        }
        break;
        case LIST_HANDLE_NAME:
        {
            uint8_t handSize = recvBuff[3];
            std::string &&handName = std::string(recvBuff+4, (recvBuff+4)+handSize);
            std::cout << handSize << std::endl;
        }
        break;
        case FINISHED_LIST_HANDLES:
        {
            std::cout << "done listing handles" <<  std::endl;
        }
        break;
        default:
            std::cout << "Non existing flag" << std::endl;
        break;
    }

}


    void TCPClient::loop(){

        ssize_t recv_len;
        ssize_t stdin_len;

        fd_set fd_inputs;
start:
        FD_ZERO(&fd_inputs);
        FD_SET(STDIN_FILENO, &fd_inputs);
        FD_SET(this->skt, &fd_inputs);
        std::cout << "$: ";
        fflush(stdout);

        uint16_t pkt_len;
        std::vector<std::string> parsedInput;
        std::queue<std::string> messages;
        // Step 1 - get user input
        
        safe_select(this->skt + 1, &fd_inputs, NULL, NULL, NULL);
        // Case 1 - something has been writen to the socket
        if(FD_ISSET(this->skt, &fd_inputs)){
            // if recv a end aka recv_len = 0
            processSocket();
        }
        if(FD_ISSET(STDIN_FILENO, &fd_inputs)){
            processUserInput();
        }

          goto start;

    }

   