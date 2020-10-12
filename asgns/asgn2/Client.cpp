#include "Client.hpp"



/***************** Client functions ****************************/
/**
 * Requirements:
 *  0.) No more than 1400 bytes including everything
 *  1.) msg must be at most 200 bytes in length
 *  2.) If msg is more than 200 bytes make sure to break it up into multiple packets
 *  3.) if no msg is given include just a new line as the message
 */


Client::Client(char *handle, int type, int protocol)  
: handle(handle)
{
    this->skt = safe_socket(AF_INET6, type, protocol);
    memset(Buff, 0, MAX_BUFF);
}
TCPClient::~TCPClient(){
    safe_close(this->skt);
}
void TCPClient::logout(){
    safe_close(this->skt);
    this->skt = -1;
}


TCPClient::TCPClient(char *handle, int protocol)  
: Client(handle,SOCK_STREAM, protocol){
}

void TCPClient::login(char* serverName, char *port){
    uint8_t * ipAddress = NULL;
    uint16_t loginLen;
    struct sockaddr_in6 server; 
    // setup the server structure
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(atoi(port));

    // get the address of the server 
    if ((ipAddress = getIPAddress6(serverName, &server)) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    safe_connect(this->skt, (struct sockaddr*)&server, sizeof(server));

    // Login

    loginLen = PacketFactory::buildLoginPacket(this);
    safe_send(this->skt, this->Buff, loginLen,0);

}
void TCPClient::forwardMCommand(std::string &input){
    MCommandParser parser;
    parser.parse(input);
    while(!parser.getMessages().empty()){
        // this below could possibly be a static function class (returns uin8t_t)
        uint16_t len = PacketFactory::buildMPacket(parser, this);
        safe_send(this->skt, this->Buff, len, 0);
        parser.getMessages().pop();
    }
}

void TCPClient::forwardBCommand(std::string &input){
        BCommandParser parser;
        parser.parse(input);
        while(!parser.getMessages().empty()){
            uint16_t len = PacketFactory::buildBPacket(parser, this);
            safe_send(this->skt, this->Buff, len, 0);
            parser.getMessages().pop();
        }
}
void TCPClient::forwardLCommand(){
    uint16_t len = 3;
    uint8_t *pkt = this->Buff;
    len = htons(len);
    memcpy(pkt, &len, 2);
    pkt[2] = LIST_HANDLES;
    safe_send(this->skt, pkt, 3, 0);
}

void TCPClient::forwardECommand(){
    uint16_t len = 3;
    uint8_t *pkt = this->Buff;
    len = htons(len);
    memcpy(pkt, &len, 2);
    pkt[2]  = CLIENT_EXIT;
    safe_send(this->skt, pkt, 3, 0);
}

void TCPClient::forwardUserInput(){
        char buff[CommandValidator::MAX_INPUT+1];
        memset(buff, 0, CommandValidator::MAX_INPUT+1);
        int enteredLen = getUserInput(buff, CommandValidator::MAX_INPUT+1);
        std::string input = buff;
        // Step 1 - read and validate input
        if(CommandValidator::validate(input, enteredLen)){
            // Step 2 - see if it has a message
            memset(Buff, 0, MAX_BUFF);
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

    std::cout << "$: " << std::flush;
    return;
}
uint16_t TCPClient::readSocket(){
    uint16_t pkt_len;
    if((pkt_len = readPacket(this->skt, this->Buff)) == 0){
        std::cout << std::endl << "Server Terminated" << std::endl;
        safe_close(this->skt);
        exit(EXIT_FAILURE);
    }
    return pkt_len;
}


void TCPClient::processSocket(){
    uint8_t flag;
    memset(this->Buff, 0, MAX_BUFF);
    uint16_t len = readSocket(); 
    switch((flag=this->Buff[2]))
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
            parser.parse(Buff, len);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage() << std::endl;
        }
        break;
        case MULTICAST:
        {
            MulticastPacketParser parser;
            parser.parse(Buff, len);
            std::cout << std::endl << parser.getSourceHandle() << ": " << parser.getMessage() << std::endl;
        }
        break;
        case MULTICAST_HANDLE_DNE:
        {
            uint8_t err_len = Buff[3];
            std::string err_han = std::string(Buff+4, Buff+4+err_len);
            std::cout << std::endl << "Client with handle " << err_han << " does not exist" << std::endl;

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
            memcpy(&numHandles, &Buff[3], 4);
            numHandles=ntohl(numHandles);
            std::cout << std::endl << "Number of handles: " << numHandles << std::endl;
        }
        break;
        case LIST_HANDLE_NAME:
        {
            uint8_t handSize = Buff[3];
            std::string &&handName = std::string(Buff+4, (Buff+4)+handSize);
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

    fd_set fd_inputs;
start:
    FD_ZERO(&fd_inputs);
    FD_SET(STDIN_FILENO, &fd_inputs);
    FD_SET(this->skt, &fd_inputs);
    safe_select(this->skt + 1, &fd_inputs, NULL, NULL, NULL);
    if(FD_ISSET(this->skt, &fd_inputs)){
        processSocket();
    }
    if(FD_ISSET(STDIN_FILENO, &fd_inputs)){
        forwardUserInput();
     }

        goto start;

}

