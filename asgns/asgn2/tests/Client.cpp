#include "Utils.hpp"
#include "Client.hpp"
#include <numeric>


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
        throw "Max input!";
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

        //TODO UN BLOCK : selectCall(this->skt, 0, 0, 1);
        // Case 1 - something has been writen to the socket
        /*
        if(FD_ISSET(this->skt, &fd_inputs)){
            // if recv a end aka recv_len = 0
            if((recv_len = processSocket()) == 0)
                return;
            else
                std::cout << "\n" << this->recvBuff << std::endl;
        }
        // Case 2 - something has been writen to stdin
        if(FD_ISSET(STDIN_FILENO, &fd_inputs)){
           std::string fd_inputs = readUserInput();
           uint16_t pkt_len = this->pb.formatPacket(input, *this);
           safe_send(this->skt, this->transBuff, pkt_len, 0);
        }
        */
           std::string input;
           try{
               input = readUserInput();
               input = UserParser::trim(input);
                if(UserParser::validateInput(input)){
                    uint16_t pkt_len = this->pb.formatPacket(input, *this);
                    safe_send(this->skt, this->transBuff, pkt_len, 0);
                }
           }catch(const char *msg){
               std::cerr << msg << std::endl;
           }

         
           goto start;

    }

   

uint16_t PacketBuilder::formatMPacket(std::vector<std::string> &args, TCPClient &client){
    
    int num_handles = std::stoi(args[0]);
    int cursor=0;
    /* args format: <num-dests> <dest-handle1> ... [dest-handlen] [text] */

    std::vector<std::string> handles;
    std::string msg;

    // Step 2 - find the handles (assumed to be inex [1,size-2])
    for (size_t i = 1; i < num_handles+1; i++){
        handles.push_back(args[i]);
    }

    // Step 3 - get msg pased[size-1] 
    // Case to see if a msg exists
    if(num_handles+1 < args.size()){
       msg = std::accumulate(args.begin()+1+num_handles, args.end(), std::string(""),
            [&](std::string &a, std::string &b){ 
                if(a.size()==0)
                    return b;
                return a+ " " +b; 
               });
    }else{
        msg = "\n";
    }
    char *src_handle = client.handle;
    uint8_t *transBuff = client.transBuff;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN + 1 + strlen(src_handle);

    for(std::string handle: handles){
        uint8_t handle_len = handle.size(); 
        pkt_len += (handle_len+1);
    }
    pkt_len+=msg.size()+1; // plus one for the null

    // TODO: no shifts Done with the length
    memcpy(transBuff+cursor, &pkt_len ,2);
    cursor+=2;

    transBuff[cursor++] = MULTICAST;
    transBuff[cursor++] = strlen(client.handle);
    // Step 2 - get the handles in the field s
    memcpy(transBuff+cursor, client.handle, strlen(client.handle));
    cursor+= strlen(client.handle);
    // Do the destination handles
    for(int i =0; i< num_handles; i++){
        transBuff[cursor++] = handles[i].size();
        memcpy(transBuff+cursor, &(handles[i]), handles[i].size());
        cursor+=handles[i].size();
    }

    // DO the msg
    memcpy(transBuff+cursor, msg.c_str(), msg.size()+1);

    return pkt_len;
}

uint16_t PacketBuilder::formatBPacket(std::vector<std::string> &args, TCPClient &client){
    uint16_t pkt_len;
    uint8_t flag = BROADCAST;
    uint8_t *transBuff = client.transBuff;
    uint8_t hand_len = strlen(client.handle);
    // args = text
    // pkt = <hdr(3)|src-len(1)|src-name|text>
    std::string message = args[0];
    pkt_len = HDR_LEN+FLAG_LEN+1+hand_len+message.size()+1;
    
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    memcpy(transBuff+3, &hand_len, 1);
    memcpy(transBuff+4, message.c_str(), message.size()+1);
    return pkt_len;
}


uint16_t PacketBuilder::formatLPacket(TCPClient &client){
    uint8_t *transBuff = client.transBuff;
    uint8_t flag = LIST_HANDLES;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN;
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    return pkt_len;
}

uint16_t PacketBuilder::formatEPacket(TCPClient &client){
    uint8_t *transBuff = client.transBuff;
    uint8_t flag = CLIENT_EXIT;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN;
    memcpy(transBuff, &pkt_len, 2);
    memcpy(transBuff+2, &flag, 1);
    return pkt_len;
}


// Assume input has no trailing or front whitespaces
uint16_t PacketBuilder::formatPacket(const std::string &input, TCPClient &client){
    uint16_t pkt_len;
    uint8_t flag;
    char cmd = input[1];
    std::string str_args = input.substr(2, input.size());
    std::vector<std::string> args = UserParser::parse(str_args);

    // Step 1 - parse to get all the fields then get size
    switch (std::toupper(cmd))
    {
    case 'M':
        pkt_len = formatMPacket(args, client);
        break;
    case 'B':
        pkt_len = formatBPacket(args, client);
        break;
    case 'L':
        pkt_len = formatLPacket(client);
        break;
    case 'E':
        pkt_len = formatEPacket(client);
        break;
    default:
        return 0;
        break;
    }
    return pkt_len;
}


