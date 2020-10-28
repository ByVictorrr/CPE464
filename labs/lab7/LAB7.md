# UDP and SendtoErr library

* This lab7 will help you with program #3

## udpClient -> `rcopy`
* Takes in an error rate, read in user input and create a PDU
* Print that PDU and then send using `sendtoErr()` the PDU to the server.

## updServer -> `server`
* Must `revfrom()` the PDU and print out the PDU that it recieves
    * maybe in a tabular format


## Overview of steps
Overview of steps:
1. Modify udpClient.c to take an error rate for dropping/corrupting packets
2. Modify udpClient.c to call sendtoErr_init()
3. Write a function to create an application level PDU in the proper format for program #3
4. Write a function to print out a PDU.
5. Modify udpClient.c to call sendtoErr() instead of sendto() and to print your PDU.
6. Modify udpServer.c to print out the PDU


### 0. Packet Format
[sequence_num(4-byte, in network order)|checksum(2bytes)|flag(1byte)|payload]

### 1. Mofiy the UdpClient.c to take an error rate 
* Error rate: rate of dropping/corrupting packets
* Intervals = [0, 1] between

### 2. Integrate the sendtoErr() library into the udpClient.c  
* Change `sendto()` -> `sendtoErr()`
    * in network.c, cpe464.h

* Add a call to `sendtoErr_init()`

