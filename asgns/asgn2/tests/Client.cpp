#include "Utils.hpp"
#include "Client.hpp"
#include <numeric>

#include <queue>
/***************** Client functions ****************************/
/**
 * Requirements:
 *  0.) No more than 1400 bytes including everything
 *  1.) msg must be at most 200 bytes in length
 *  2.) If msg is more than 200 bytes make sure to break it up into multiple packets
 *  3.) if no msg is given include just a new line as the message
 */
#define MAX_USER_INPUT 1400
std::string Client::readUserInput(){
    uint8_t c;
    std::string input;
    std::getline(std::cin, input);
    // Case 1 - user didnt enter in anything
    if(input.size() >= MAX_USER_INPUT-1)
        throw "Max input! - Please enter in less than 1400 chars";
    return input;
}
size_t Client::processSocket(){
    // step 1 - if server closes the connection
    if(safe_recv(this->skt, this->recvBuff, MAX_BUFF, 0) < 0) {
        return 0;
    }
    // step 2 - else read the data and fomat
    printf("%s", this->recvBuff);
    memset(this->recvBuff, 0, MAX_BUFF);
}



Client::Client(char *handle, char *server_name, char *port, int type, int protocol)  
: handle(handle), serverName(server_name), port(port)
{
    this->skt = safe_socket(AF_INET6, type, protocol);
    memset(recvBuff, 0, MAX_BUFF);
    memset(transBuff, 0, MAX_BUFF);
}
Client::~Client(){
    safe_close(&this->skt);
}
void Client::close(){
    safe_close(&this->skt);
}


TCPClient::TCPClient(char *handle, char *server_name, char *port, int protocol)  
: Client(handle, server_name, port, SOCK_STREAM, protocol){
    safe_close(&this->skt);
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
    safe_send(this->skt, this->handle, strlen(this->handle),0);

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString(ipAddress), atoi(this->port));
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
        memset(this->recvBuff, 0, MAX_BUFF);
        memset(this->transBuff, 0, MAX_BUFF);
        std::cout << "$: ";

        std::string input;
        uint16_t pkt_len;
        std::vector<std::string> parsedInput;
        std::queue<std::string> messages;
        // Step 1 - get user input
        try{
                input = readUserInput();
                /* Step 0 - Validate and make sure stdin has a 
                            command format as well as have less than 
                */
                if(CommandValidator::validateInput(input)){
                    // Step 1- check to see if M or B msg
                    input = CommandParser::trim(input);
                    // Step 2 - see if it has a message
                    switch (CommandParser::getCommand(input))
                    {
                    case 'M':
                        MCommandParser parser;
                        parser.parse(input);
                        while(messages.size() > 0){
                            // this below could possibly be a static function class (returns uin8t_t)
                            MPacketBuilder builder(num_handles, dest_handles, messages[0]);
                        }
                        break;
                    case 'B':
                        break;
                    case 'L':
                        break;
                    
                    default:
                        break;
                    }

                }
          }catch(const char *msg){
            std::cerr << msg << std::endl;
          }
           goto start;

    }

   