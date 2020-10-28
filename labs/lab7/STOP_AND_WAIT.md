# CPE 464 - Stop and Wait Design

## Objective
* Design a remote file copy command using UDP(server and client).
	* `rcopy` - is the client program
		* `./rcopy <remote-file name> <local-file name>`
		* It will recieve packets from the `<remote-file name>` and write the data to `<local-file name>`
	* `server` - is the server program
		* `./server`
		* recieves the `<remote-file name>` from `rcopy` and then tranmit the data from remote-file back to `rcopy`
**Basically to get a file from the server onto the client using UDP (provding reliable connection between two programs)**


## Details
### Sending Errors
* A function provided will randomly throw away packets
* Instead of using `sendto()` the function used should be called `sendtoErr()`
	* This function should be used for all communication between client and server
	* It allows client and server to drop some of the data packets and flip some bits
		* The probability that the server will drop a packet/flip a bit is a run time argument (error-percentage)
### File transfer
* In order to complete the file transfer you will need to implement stop-and-wait flow control algorithm
	* Every recieved packet should be acknowledged before sending another packet
	* In order to recover from lost data you will set a timer (via the select function)

### Check Errors
* To detect bit flips you will use the internet checksum algorithm.
	* If an error is detected use recover measures

### Packet format

|  Sequence Number  | Internet Checksum  |    Flag    | Data                                |
|:-----------------:|:------------------:|:----------:|-------------------------------------|
| <----4 bytes----> | <-----2 bytes----> | <-1 byte-> | <-buffer-size(check `rcopy` arg)->  |

**Data examples: file data, filename or whatever payload your trying to send**

## `rcopy`
* This program is responsible for taking the two file names as cmd line arguments and commuicating with the `server` program to request a `<remote-file>`
* It will recieve the file data from the server and store it to disk using the `<local-file name>`
```console
$ ./rcopy from-remote-file to-local-file buffer-size error-percent remote-machine remote-port
```
**from-remote-file**: is the file on the remote machine to be copied

**to-local-file**: is the file copied into on the local machine

**buffer-size**: is the number of data bytes (from the file) transmitted in a packet

**error-percent**: is the percent of packets that will have errors (floating point number)

**remote-machine**: is the remote machine running the server

**remote-port**: is the port number of the server application

## `server`
* This program is responsible for accepting requests for files from `rcopy` program and transmitting the file back.
* This program should never terminate (unless you kill it with a ctrl-c).
* It should continually process requests from client (`rcopy`) programs.
```console
$ ./server error-percent [optional-port-number]
```
**error-percent**: is the percent of packets that will be dropped (floating point number)

**optional-port-number**: port number for the server to use (if not present the server should use a port number assigned by the operating system)

**The server should output its port number to be used by the rcopy program**

## Requirements

1. The `buffer-size` parameter should be sent from `rcopy` to the server prior to the file transfer.
	* The server uses this `buffer-size` to determine how much data in the file to send in each packet

2. The server needs to handle rcopy requests for non-existing files by sending back to the `rcopy` prgoram a packet with a flag indicating this error.

3. When you send a data packet (a packet that contains data from the file) the sequence number MUST be in network order, the sequence number should start at 0 and grow every time a NEW packet is transmitted.
	* Retransmitted data packets should have the same sequence number as the orginal packet.

4. In order to prevent the server program from blocking forever the `select(..)` function must be used.
	* This function allows the server to "probe" the socket for a specified of time.
	* If `select(..)` times out the server should assume the data was lost and retransmit the packet ( this timeout period is 1s )

5. During the filename tranfer (so sending of the filename to the server)
	* If the client retransmits a filename packaet 10 time ( so 10 consecutive losses of the filename) the client can ssume the server is unreachable and terminate with an error message `"server unreachable"`
	* rcopy should use a 1-second timeout when sending the filename and waiting on the filename ACK

### Timing summary

#### Reciever
* if a program is waiting on something other than an ACK/RR/Filename Confirmation, it should call `select()` with a timeout value of 10s. If no data has arrived after 10s, the program can assume the other side is down (terminated)
#### Sender
* You should hould set a timer(1s) for sending program (using `select`)
	* If this timer runs out then then go into error control (e.g resending the packet)
	* This program should terminate if it calls select() 10 times and has not heard back from the other side

### Flags
* These are used inside your header

	a. Flag = 1 Data packet b

	b. Flag = 2 ACK

	c. Flag = 3 Packet contains the file name/buffer size (rcopy to server)

	d. Flag = 4 File OK on server (server to rcopy)

	e. Flag = 5 File not found on server (server to rcopy)

### Error Handling
* if the remote file doesnt exist the `rcopy` shoudl print out an error message and terminate gracefully.
	* The server shouldnt terminate

### Usage

```

On Unix1:
$ ./server .01 54321
Server is using port 54321

On Unix2:
$ ./rcopy myfile1 myfile2 1000 .01 unix1 54321
Error myfile1 does not exist on unix1>
rcopy myfile3 myfile2 1000 .01 unix1 54321
