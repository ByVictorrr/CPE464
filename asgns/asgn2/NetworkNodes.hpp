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
// Helps the server class read flags and  (might be a static class)
class ChatPDUPaser{
    private:
        /* Format: 
            Flag: #
            From: [server | client]
            To: [server | client]
            Desc: <Desc>
            Format: <hdr (# bytes) | field1 (# bytes) | .. | fieldn(# bytes)>
        */
        static enum FLAGS{
            /**
                Flag: 0
                From: NULL
                To: NULL
                Desc: Not used
                Format: NULL
            **/
            NOT_USED,  
            /**
                Flag: 1
                From: client
                To: server
                Desc: sends over the handle name to make sure its valid
                Format: <hdr (3) | handle-len (1) | handle-name(handle-len)>
            **/
            CHECK_HANDLE,  
            /**
                Flag: 
                From: client
                To: server
                Desc: sends over the handle name to make sure its valid
                Format: <hdr (3) | handle-len (1) | handle-name(handle-len)>
            **/
            GOOD_HANDLE, // Server confirms that the handle is valid 

        }
        static uint8_t flag;
    public:
       /**
        * Parses incoming pdu and then returns the flag
        */
        static uint8_t parsePDU(uint8_t *pdu){
            uint8_t flag;
            switch ((flag=pdu[HDR_LEN]))
            {
            case /* constant-expression */:
                /* code */
                break;
            
            default:
                break;
            }
        }

        

};




#endif