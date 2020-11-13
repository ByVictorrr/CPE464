#ifndef SAFE_SYS_CALLS_H
#define SAFE_SYS_CALLS_H
extern "C"{

#include <stdio.h>
void *safe_calloc(size_t nmemb, int size);
void *safe_realloc(void *src, int size);
void safe_free(void **buff);
void safe_close(int fd);
int safe_socket(int domain, int type, int protocol);
int safe_bind(int skt, struct sockaddr *addr, socklen_t addrlen);
int safe_getsocketname(int skt, struct sockaddr *addr, socklen_t *addrlen);
int safe_listen(int skt, int backlog);
int safe_accept(int server_socket, struct sockaddr * addr, socklen_t *addrlen);
int safe_connect(int skt, const struct sockaddr *addr, socklen_t addrlen);
int safe_send(int skt, const void *buf, size_t len, int flags);
ssize_t safe_recv(int skt, void *buf, size_t len, int flags);
int safe_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
uint16_t readPacket(int skt, uint8_t *buff);
int send_pkt(int skt, const void *buf, size_t len);
int safe_accept_client(int server_socket, int debugFlag);

FILE *safe_fopen(const char *filename, const char *mode);
int safe_fclose(FILE *stream);
pid_t safe_fork();
}
#endif