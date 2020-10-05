#include "Utils.hpp"
#include "Client.hpp"

/***************** Client functions ****************************/
/**
 * Gets input from a user and then returns the length of the input
 */
std::string Client::readUserInput(){
    uint8_t c;
    std::string input;
    
    //int place = (HDR_LEN+FLAG_LEN-1);
    int place = 0;
    int len;
    // get input
    std::getline(std::cin, input);
    if(input.size() >= MAX_BUFF+HDR_LEN+FLAG_LEN-1)
        return nullptr;
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
           std::string input = readUserInput();
           std::string parsed_input = std::regex_replace(input, std::regex("^ +| +$|( ) +"), "$1");
           uint16_t pkt_len = this->pb.formatPacket(input, *this);
           safe_send(this->skt, this->transBuff, pkt_len, 0);

        goto start;

    }

   

uint16_t PacketBuilder::formatMPacket(std::string &args, TCPClient &client){
    
    int num_handles;
    int cursor=0;
    /* args format: <num-dests> <dest-handle1> ... [dest-handlen] [text] */
    std::vector<std::string> parsed = splitWhitespace(args);

    std::vector<uint8_t> handle_lens;
    std::vector<std::string> handles;
    std::string msg;

    // Step 1 - check to see if num-handles is there 
    if(!(num_handles = std::atoi(parsed[0].c_str())) || num_handles < 0 || num_handles > 9){
        std::cerr << "Usage: %[m|M] num-handles(1-9) dest-handle1 .. [dest-handle9] [text]" << std::endl;
        return false;
    }
    // Step 2 - find the handles (assumed to be inex [1,size-2])
    for (size_t i = 1; i < num_handles+1; i++){
        handles.push_back(parsed[i]);
        handle_lens.push_back(handles[i-1].length());
    }

    // Step 3 - get msg pased[size-1] 
    // Case to see if a msg exists
    if(2*num_handles+1 < parsed.size()){
        // msg = parsed.substr();
    }else{
        msg = "\n";
    }
    char *src_handle = client.handle;
    uint8_t *transBuff = client.transBuff;
    uint16_t pkt_len = HDR_LEN+FLAG_LEN;

    for(uint8_t handle_len: handle_lens){
        pkt_len += (handle_len+1);
    }
    pkt_len+=msg.size()+1; // plus one for the null

    // Done with the length
    transBuff[cursor++] = pkt_len >> 8;
    transBuff[cursor++] = pkt_len;
    transBuff[cursor++] = MESSAGE;
    transBuff[cursor++] = strlen(client.handle);
    // Step 2 - get the handles in the field s
    memcpy(transBuff+cursor, client.handle, strlen(client.handle));
    cursor+= strlen(client.handle);
    // Do the destination handles
    for(int i =0; i< num_handles; i++){
        transBuff[cursor++] = handle_lens[i];
        memcpy(transBuff+cursor, &(handles[i]), handles[i].size());
        cursor+=handles[i].size();
    }

    // DO the msg
    memcpy(transBuff+cursor, &msg, msg.size()+1);

    return pkt_len;
}


// Assume input has no trailing or front whitespaces
uint16_t PacketBuilder::formatPacket(std::string &input, TCPClient &client){
    uint16_t pkt_len;
    uint8_t flag;
    char cmd = input[1];
    std::string args;

    // Case 2 - stdin has a command
    if( input.size() < 2 || input[0] != '%' || !inCommnds(input[1])){
        std::cout << "Not a valid command" << std::endl;
        return NULL;
    }
    args = input.substr(2, input.size());
    // Step 1 - parse to get all the fields then get size
    switch (toupper(cmd))
    {
    case 'M':
        pkt_len = formatMPacket(args, client);
        break;
    
    default:
        break;
    }
    return pkt_len;
}


