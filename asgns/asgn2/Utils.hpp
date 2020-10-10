#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"
#include <vector>
#include <sstream>
#include <regex>
#include <queue>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10



typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MULTICAST,
    UN_USED,
    MULTICAST_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;


std::string trim_left(const std::string& str);
std::string trim_right(const std::string& str);
std::string trim(const std::string& str);
std::vector<std::string> splitByWhiteSpace(std::string &input);
std::queue<std::string> split(const std::string str, int splitLen);
int getFirstDigit(std::string &str)throw(const char *);

#endif
