#ifndef NETWORK_NODES_H
#define NETWORK_NODES_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"

#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10
#define TIME_IS_NOT_NULL 0

int selectCall(int socketNumber, int seconds, int microseconds, int timeIsNotNull);





#endif