#include "NetworkNodes.hpp"
#include "Server.hpp"


/* Server class */
Server::Server(int max_clients, int port, int type, int protocol=0)  
: port(port)
{
    this->skt=safe_socket(AF_INET6, type, protocol);
    this->client_sks = new int[max_clients];
}
Server::~Server(){
    delete [] this->client_sks;
    safe_close(&this->skt);
}



/* TCP server calss */
TCPServer::TCPServer(int max_clients, int port, int protocol=0)  
: Server(max_clients, port, SOCK_STREAM, protocol){}

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

int TCPServer::acceptClient(int server_socket, int debugFlag)
{
    struct sockaddr_in6 clientInfo;   
    int clientInfoSize = sizeof(clientInfo);
    int client_socket= 0;
    client_socket = safe_accept(server_socket, (struct sockaddr*) &clientInfo, (socklen_t *) &clientInfoSize);
    
    if (debugFlag)
    {
        // TODO change to std::cout
        printf("Client accepted.  Client IP: %s Client Port Number: %d\n",  
                getIPAddressString(clientInfo.sin6_addr.s6_addr), ntohs(clientInfo.sin6_port));
    }
    
    return(client_socket);
}