#include "safe_sys_calls.h"
#include <stdint.h>

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
void safe_free(char **buff)
{
    if(*buff != NULL){
        free(*buff);
		*buff=NULL;
	}
    return;
}
void safe_close(int *fd){
	if(*fd >= 0){
		close(*fd);
	}
	*fd = -1;
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
int safe_selectCall(int socketNumber, int seconds, int microseconds, int timeNull)
{
	// Returns 1 if socket is ready, 0 if socket is not ready  
	// set timeIsNotNull = TIME_IS_NOT_NULL when providing a time value
	int numReady = 0;
	fd_set fileDescriptorSet;  // the file descriptor set 
	struct timeval timeout;
	struct timeval * timeoutPtr;   // needed for the time = NULL case
	
	
	// setup fileDescriptorSet (socket to select on)
	  FD_ZERO(&fileDescriptorSet);
	  FD_SET(socketNumber, &fileDescriptorSet);
	
	// Time can be NULL, 0 or a seconds/microseconds 
	if (!timeNull)
	{
		timeout.tv_sec = seconds;  
		timeout.tv_usec = microseconds; 
		timeoutPtr = &timeout;
    } else
    {
		timeoutPtr = NULL;  /* block forever - until input */
    }

	numReady = safe_select(socketNumber + 1, &fileDescriptorSet, NULL, NULL, timeoutPtr);
	// We have to wait for a response after we send something
  
	// will only by 1 or 0
    return numReady;
}

uint16_t read_pkt(int skt, uint8_t *buff){
    uint16_t pkt_len;
    if(safe_recv(skt, &pkt_len, 2, MSG_WAITALL) < 0){
        safe_close(skt);
        return 0;
    }
    if(safe_recv(skt, buff+2, pkt_len-2, 0) < 0){
        safe_close(skt);
        return 0;
    }
    memcpy(buff, &pkt_len, 2);
    return pkt_len;
 
}