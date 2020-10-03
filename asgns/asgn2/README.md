## Program 2 - Chat 

## Overview 
* This will be a server and client(s) chat program 
    * The server acts as a middle man between clients

**Note: this is graded using a manual script**

## Requirments
1. No more than one malloc call during a program execution
2. Should not use shifting
3. You need to use system calls - make sure check return value
    * TCP is implemented in the unix kernel
4. After close, you cant sned data anymore but other side may recieve data. Related to `sliding window flow control`
5. Dont use read/write for sockets(use recv/send)

### Chat protocol (tcp guarentees order arrived)
* may group data tother which is the probelm that it needs to handle - 3 cases:
    1. sent as is
    2. combined with another packet
    3. broken up into two packets

6. No pthreads or fork in select
7. use recfrom and sento

## Description
* create your own level packet and use tcp to send the packet.
### Format of PDU
* All packets you create will start with the same first 3 bytes(chat-hdr)
#### Chat header
* 2 bytes PDU length in network order (payload + header)
* 1 byte flag
#####


