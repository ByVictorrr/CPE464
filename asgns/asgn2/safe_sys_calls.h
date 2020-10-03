#ifndef SAFE_SYS_CALLS_H
#define SAFE_SYS_CALLS_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
void *safe_calloc(size_t nmemb, int size);
void *safe_realloc(void *src, int size);
void safe_free(char **buff);
void safe_close(int *fd);
int safe_socket(int domain, int type, int protocol);
int safe_bind(int skt, struct sockaddr *addr, socklen_t addrlen);
int safe_getsocketname(int skt, struct sockaddr *addr, socklen_t *addrlen);
int safe_listen(int skt, int backlog);
int safe_accept(int server_socket, struct sockaddr * addr, socklen_t *addrlen);
int safe_connect(int skt, const struct sockaddr *addr, socklen_t addrlen);
int safe_send(int skt, const void *buf, size_t len, int flags);
ssize_t safe_recv(int skt, void *buf, size_t len, int flags);
	
#endif