#include "safe_sys_calls.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void *safe_calloc(size_t nmemb, int size){
	void *ret;
	if((ret=calloc(nmemb, size)) < 0){
		perror("calloc error");
		exit(EXIT_FAILURE);
	}
	return ret;
}
void *safe_realloc(void *src, int size){
	void *ret;
	if((ret=realloc(src, size)) < 0){
		perror("realloc error");
		exit(EXIT_FAILURE);
	}
	return ret;
}
void safe_free(void **buff)
{
    if(*buff != NULL){
        free(*buff);
		*buff=NULL;
	}
    return;
}
void safe_close(int fd){
	if(fd < 0){
		return;	
	}else if(close(fd) < 0){
		perror("close error");
		exit(EXIT_FAILURE);
	}
}
int safe_socket(int domain, int type, int protocol){
    int skt;
    if((skt=socket(domain,type, protocol)) < 0){
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    return skt;
}
// bind the name to the socket  (name the socket)
int safe_bind(int skt, struct sockaddr *addr, socklen_t addrlen){
    if (bind(skt, addr, addrlen) < 0)
    {
        perror("bind call");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int safe_getsocketname(int skt, struct sockaddr *addr, socklen_t *addrlen){
    if (getsockname(skt, addr, addrlen) < 0)
	{
		perror("getsockname call");
		exit(EXIT_FAILURE);
    }
    return 0;
}
int safe_listen(int skt, int backlog){
	if (listen(skt, backlog) < 0)
	{
		perror("listen call");
		exit(EXIT_FAILURE);
	}
	return 0;	
}

int safe_accept(int server_socket, struct sockaddr * addr, socklen_t *addrlen){

	int client_socket;
	if ((client_socket = accept(server_socket, addr, addrlen)) < 0)
	{
		perror("accept call error");
		exit(EXIT_FAILURE);
	}
	return client_socket;	

}
int safe_connect(int skt, const struct sockaddr *addr, socklen_t addrlen){
	if(connect(skt, addr, addrlen) < 0)
    {
		perror("connect call");
		exit(EXIT_FAILURE);
	}
	return 0;
}
int safe_send(int skt, const void *buf, size_t len, int flags){
	if(send(skt, buf, len, flags) < 0)
	{
		perror("send call");
		exit(EXIT_FAILURE);
	
	}
	return 0;
}
ssize_t safe_recv(int skt, void *buf, size_t len, int flags){
	ssize_t recv_len;	
	if((recv_len=recv(skt, buf, len, flags)) < 0)
	{
		safe_close(skt);
		perror("recv call");
		exit(EXIT_FAILURE);
	
	}
	return recv_len;
}
int safe_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
	int numReady;
	if ((numReady = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
	{
		perror("select call");
		exit(EXIT_FAILURE);
    }
  
	// Will be either 0 (socket not ready) or 1 (socket is ready for read)
    return numReady;
}
uint16_t readPacket(int skt, uint8_t *buff){
    uint16_t pkt_len, net_len;
    if(safe_recv(skt, &net_len, 2, MSG_WAITALL) == 0){
		return 0;
	}
    pkt_len = ntohs(net_len);
    if(safe_recv(skt, buff+2, pkt_len-2, MSG_WAITALL) == 0){
        return 0;
    }
    memcpy(buff, &pkt_len, 2);
    return pkt_len;
}

int send_pkt(int skt, const void *buf, size_t len){
	return safe_send(skt, buf, len, 0);
}

int safe_accept_client(int server_socket, int debugFlag)
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

FILE *safe_fopen(const char *filename, const char *mode){
	FILE *fp;
	if((fp=safe_fopen(filename, mode)) == NULL){
		fprintf(stderr, "Error opening: %s", filename)
		return NULL;
	}
	return fp;
}
int safe_fclose(FILE *stream){
    if(fclose(stream) == EOF){
        fprintf(stderr, "Error: unable to close");
        return EOF;
    }
    return 1;
}