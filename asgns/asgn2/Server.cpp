#include "Server.hpp"



Server::Server(int port, int type, int protocol)  
: port(port)
{
    this->skt=safe_socket(AF_INET6, type, protocol);
    this->clients = std::map<int, std::string>();
}

/* TCP server calss */
TCPServer::TCPServer(int port, int protocol)  
: Server(port, SOCK_STREAM, protocol){}

TCPServer::~TCPServer(){
    this->shutdown();
}
void TCPServer::config(){
    struct sockaddr_in6 server;      /* socket address for local side  */
    socklen_t len= sizeof(server);  /* length of local address        */
    // setup the information to name the socket
    server.sin6_family= AF_INET6;         		
    server.sin6_addr = in6addr_any;   //wild card machine address
    server.sin6_port= htons(port);         
    // bind the name to the socket  (name the socket)
    safe_bind(this->skt, (struct sockaddr*)&server, sizeof(server));
    safe_getsocketname(this->skt,(struct sockaddr*)&server, &len);
    safe_listen(this->skt, BACKLOG);
    std::cout << "Server Port Number " + std::to_string(ntohs(server.sin6_port)) << std::endl;
}


bool TCPServer::inClients(const std::string &handle){
    for(const auto & p: this->clients){
        std::string client = p.second;
        if(client.compare(handle) == 0)
            return true;
    }
    return false;
}
int TCPServer::getSocket(const std::string &handle){
    for(const auto & p: this->clients){
        std::string client = p.second;
        if(client.compare(handle) == 0)
            return p.first;
    }
    return -1;
}

/********************Forwarder functions***************/
void TCPServer::forwardListHandles(int clientSocket){
        int numHandles = this->clients.size();
        uint16_t len = 3 + numHandles;
        // step 1 - send the num of packets (with flag=11) 4 byte num
        memcpy(transBuff, &len, 2);
        transBuff[2] = NUM_HANDLES;
        memcpy(&transBuff[3], &numHandles, sizeof(int));
        safe_send(clientSocket, transBuff, len, 0);
        len=0;
        memset(transBuff, 0, MAX_BUFF);
        // step 2 - send the name of each packet (hdr flag=12, 1 byte handle length, handle data)
        for(const auto & client: clients){
            len = (3 + 1+1) + client.second.size();
            memcpy(transBuff, &len, 2);
            transBuff[2] = LIST_HANDLE_NAME;
            transBuff[3] = client.second.size();
            memcpy(transBuff+4, client.second.c_str() ,client.second.size()+1);
            safe_send(clientSocket, transBuff, len, 0);
            memset(transBuff, 0, MAX_BUFF);
        }
        // Step 3 - send packet with flag = 13 know that its finished listing
        len = 3;
        memcpy(transBuff, &len, 2);
        transBuff[2] = FINISHED_LIST_HANDLES;
        safe_send(clientSocket, transBuff, len, 0);
 
}

void TCPServer::forwardCheckHandle(int clientSocket){
    LoginPacketParser parser;
    parser.parse(this->recvBuff);
    uint16_t len=3;
    memcpy(transBuff, &len, 2);
    if(this->inClients(parser.getHandName())){
        transBuff[2] = HANDLE_EXISTS;
    }else{
        transBuff[2] = HANDLE_DNE;
        this->clients[clientSocket] = parser.getHandName();
    }
    send_pkt(clientSocket, transBuff, 3);
}
void TCPServer::forwardBroadcast(int clientSocket, uint16_t pkt_len){
    BroadcastPacketParser parser;
    parser.parse(this->recvBuff);
    for(const auto &client: clients)
        if(client.second.compare(parser.getSourceHandle()) != 0)
            send_pkt(client.first, recvBuff, pkt_len);

}

void TCPServer::forwardMulticast(int clientSocket, uint16_t pkt_len){
    MulticastPacketParser parser;
    parser.parse(this->recvBuff);
    
    for(const auto &dest: parser.getDestHandles())
        if(this->inClients(dest)){
            safe_send(this->getSocket(dest), recvBuff, pkt_len, 0);
        }else{
            // creat a error pkt and send
            uint16_t err_len = 3+dest.size()+1;
            memcpy(transBuff, &err_len, 2);
            transBuff[2] = MULTICAST_HANDLE_DNE;
            transBuff[3] = dest.size();
            memcpy(transBuff+4, dest.c_str(), dest.size());
            send_pkt(clientSocket, transBuff, err_len);
        }

}

void TCPServer::forwardExit(int clientSocket){
    recvBuff[2] = CLIENT_EXIT_ACK;
    safe_send(clientSocket, recvBuff, 3, 0);
    this->clients.erase(clientSocket);
 
}


void TCPServer::processClient(int clientSocket){
    // something has been sent to the server
    uint8_t flag;
    uint16_t pkt_len;
    memset(this->transBuff, 0, MAX_BUFF);
    memset(this->recvBuff, 0, MAX_BUFF);
    // Case where client disconnects (async)
    if(!(pkt_len = read_pkt(clientSocket, this->recvBuff))){
        safe_close(clientSocket);
        this->clients.erase(clientSocket);
        return;
    }
    // once we have the packet what do we do ? (parse it look for destination or flags)
    switch ((flag=recvBuff[2]))
    {
    case CHECK_HANDLE:
        forwardCheckHandle(clientSocket);
    break;
    case BROADCAST:
        forwardBroadcast(clientSocket, pkt_len);
        break;
    case MULTICAST:
        forwardMulticast(clientSocket, pkt_len);
        break;
    case CLIENT_EXIT:
        forwardExit(clientSocket);
       break;
    case LIST_HANDLES:
        forwardListHandles(clientSocket);
        break;
    default:
        std::cout  << "Undefined flag" << std::endl;
        break;
    }
}

void TCPServer::shutdown(){
    for(std::map<int,std::string>::iterator it = this->clients.begin(); it != this->clients.end();){
        int clientSocket = it.operator*().first;
        it++;
        this->clients.erase(clientSocket);
        safe_close(clientSocket);
    }
    safe_close(this->skt);
    this->skt = -1;
}

void TCPServer::run(){
     fd_set fd_inputs;
     int max_fd = this->skt;
start:
    // step 1 - clear fd_inputs
    FD_ZERO(&fd_inputs);
    FD_SET(this->skt, &fd_inputs);
    for(const auto &item: this->clients){
        FD_SET(item.first, &fd_inputs);
    }
    // Step 2 - get the max file descriptor and call select
    max_fd = (*std::max_element(this->clients.begin(), this->clients.end(), 
        [&](auto p1, auto p2){ return p1.first < p2.first; })).first;

    if(max_fd < this->skt){
        max_fd = this->skt;
    }
    safe_select(max_fd+1, &fd_inputs, NULL, NULL, NULL);
    // Step 3 - check to see if a new client wants to connect
    if(FD_ISSET(this->skt, &fd_inputs)){
        int new_client;
        new_client = safe_accept_client(this->skt, 1);
        this->clients.insert({new_client, ""});
    }
    // Step 4 - check the file descriptors (clients)
    for(std::map<int,std::string>::iterator it = this->clients.begin(); it != this->clients.end();){
        int clientSocket = it.operator*().first;
        it++;
        if(FD_ISSET(clientSocket, &fd_inputs))
            processClient(clientSocket);
    }
 
    goto start;

}