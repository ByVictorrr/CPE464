#include "Client.hpp"



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
TCPClient::~TCPClient(){
    safe_close(this->skt);
}
void TCPClient::logout(){
    safe_close(this->skt);
    this->skt = -1;
}


TCPClient::TCPClient(char *handle, char *server_name, char *port, int protocol)  
: Client(handle, server_name, port, SOCK_STREAM, protocol){
}

void TCPClient::login(int debugFlag){
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

    // Login
    uint8_t *login_pkt = PacketFactory::buildLoginPacket(this);
    safe_send(this->skt, login_pkt, PacketParser::getPacketLen(login_pkt),0);

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString(ipAddress), atoi(this->port));
    }

}
void TCPClient::forwardMCommand(std::string &input){
    MCommandParser parser;
    parser.parse(input);
    while(!parser.getMessages().empty()){
        // this below could possibly be a static function class (returns uin8t_t)
        uint8_t *pkt = PacketFactory::buildMPacket(parser, this);
        safe_send(this->skt, pkt, PacketParser::getPacketLen(pkt), 0);
        parser.getMessages().pop();
    }
}

void TCPClient::forwardBCommand(std::string &input){
        BCommandParser parser;
        parser.parse(input);
        while(!parser.getMessages().empty()){
            uint8_t *pkt = PacketFactory::buildBPacket(parser, this);
            safe_send(this->skt, pkt, PacketParser::getPacketLen(pkt), 0);
            parser.getMessages().pop();
        }
}
void TCPClient::forwardLCommand(){
    uint16_t len = 3;
    uint8_t *pkt = this->transBuff;
    memcpy(pkt, &len, 2);
    pkt[2] = LIST_HANDLES;
    safe_send(this->skt, pkt, 3, 0);
}

void TCPClient::forwardECommand(){
    uint16_t len = 3;
    uint8_t *pkt = this->transBuff;
    memcpy(pkt, &len, 2);
    pkt[2]  = CLIENT_EXIT;
    safe_send(this->skt, pkt, 3, 0);
}

void TCPClient::forwardUserInput(std::string &input){
    try{
        // Step 1 - read and validate input
        if(CommandValidator::validate(input)){
            // Step 2 - see if it has a message
            switch (CommandParser::getCommand(input))
            {
            case 'M':
                forwardMCommand(input);
                break;
            case 'B':
                forwardBCommand(input);
                break;
            case 'L':
                forwardLCommand();
                break;
            case 'E':
                forwardECommand();
                break;
            default:
                std::cout << "Non existing flag" << std::endl;
            break;
            }

        }
    }catch(const char *msg){
        std::cerr << msg << std::endl;
    }

    std::cout << "$: " << std::flush;
    return;
}

void TCPClient::processSocket(){
    uint16_t pkt_len;
    uint8_t flag;
    uint8_t *data;
    memset(recvBuff, 0, MAX_BUFF);
    if((pkt_len = read_pkt(this->skt, this->recvBuff)) == 0){
        std::cout << std::endl << "Server closed: Exiting!" << std::endl;
        safe_close(this->skt);
        exit(EXIT_FAILURE);
    }
    switch((flag=recvBuff[2]))
    {

        case HANDLE_DNE:
            std::cout << std::endl << "Login sucessful" << std::endl;
        break;
        case HANDLE_EXISTS:
            std::cout << std::endl << "Handle name already exits" << std::endl;
            safe_close(this->skt);
            exit(EXIT_FAILURE);
        break;
        case BROADCAST:
        {
            BroadcastPacketParser parser;
            parser.parse(recvBuff);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage() << std::endl;
        }
        break;
        case MULTICAST:
        {
            MulticastPacketParser parser;
            parser.parse(recvBuff);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage() << std::endl;
        }
        break;
        case MULTICAST_HANDLE_DNE:
        {
            uint16_t len = PacketParser::getPacketLen(recvBuff);
            uint8_t err_len = recvBuff[3];
            std::string err_han = std::string(recvBuff+4, recvBuff+4+err_len);
            std::cout << std::endl << "The handle  \"" << err_han << "\" not found on the server" << std::endl;

        }
        break;
        case CLIENT_EXIT_ACK:
        {
            std::cout << std::endl << "Successfully Exiting!" << std::endl;
            safe_close(this->skt);
            exit(EXIT_SUCCESS);
        }
        break;
        case NUM_HANDLES:
        {
            int numHandles;
            // print out the number
            memcpy(&numHandles, &recvBuff[3], 4);
            std::cout << std::endl << "Number of handles: " << numHandles << std::endl;
        }
        break;
        case LIST_HANDLE_NAME:
        {
            uint8_t handSize = recvBuff[3];
            std::string &&handName = std::string(recvBuff+4, (recvBuff+4)+handSize);
            std::cout << "\t" << handName << std::endl;
        }
        break;
        case FINISHED_LIST_HANDLES:
        {
            std::cout << "Done listing handles" <<  std::endl;
        }
        break;
        default:
            std::cout << std::endl << "Non existing flag" << std::endl;
        break;
    }


}


void TCPClient::chat(){

    ssize_t recv_len;
    ssize_t stdin_len;

    fd_set fd_inputs;
start:
    FD_ZERO(&fd_inputs);
    FD_SET(STDIN_FILENO, &fd_inputs);
    FD_SET(this->skt, &fd_inputs);
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
        std::string input;
        std::getline(std::cin, input);
        forwardUserInput(input);
    }

        goto start;

}

