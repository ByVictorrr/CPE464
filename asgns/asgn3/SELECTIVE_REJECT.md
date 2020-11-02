# Selective Reject

* You are writing a remote file copy command using `UDP`
	* To achieve this two programs will be implemented: **`rcopy`** and **`server`**
	* The client will enter in local-file and remote-file and will copy the remote-file to the local-file off the server
* This program will use a sliding window flow control algorithm using Select-Reject ARQ

**We want to in induce errors therefore you have to use function `sendtoErr`, which will drop and flip bits in your data**

	* To check for bits fliped use the checksum in the `rcopy header`


## rcopy
* Responsible for taking two filenames as command line arguments and communicating with the server program to download the `from-file` from the server to the `to-file` created by rcopy.

### Usage
```console
$ ./rcopy <from-filename> <to-filename> <window-size> <buffer-size> <error-percent> <remote-machine> <remote-port>
```
**from-filename**: is the name of the file to download from the server (server sends this down)

**to-filename**: is the name of file created by rcopy (rcopy writes to this file)

**window-size**: is the size of the window in PACKETS (i.e. number of packets in window)

**buffer-size**: is the number of data bytes (from the file) transmitted in a data packet

**error-percent**: is the percent of packets that are in error (floating point number)

**remote-machine**: is the remote machine running the server

**remote-port**: is the port number of the server application

**Note: if `rcopy` cant open the <to-filename>, rcopy should print an error message: "Error on open for output of file: `<to-filename>`"**

## server
* This program is responsible for downloading the requested file to `rcopy`
	* Should never terminate (unless you kill it with ctrl-c)
	* Must process multiple clients simultaneously
	* Handle error conditions such as an error opening the file to download by sending back an error packet (error flag) to rcopy program

### Usage
```console
$ ./server <error-percent> <optional-port-number>
```
**error-percent**: is the percent of packets that are in error (floating point number)

**optional-port-number**: allows the user to specify the port number to be used by the server
(If this parameter is not present you should pass a 0 (zero) as the
port number to bind().)


**Note: if cannot open the `<from-filename>` then give rcopy an error packet to print: "Error: file `<from-filename>` not found on the server"**

## Window specifics
* By using sliding window flow control, the amount of data sent can be increased by allowing multiple frames to be in transit at the same time.
	* Never have to stop sending to wait on an RR
* The sender will maintain window-size in which packets are buffered until they are acked
* `<Window-size>`: is the limit on the # of unacked frames from sender can send before stopping and waiting for an ack.

### Determining errrors (sender and reciever)
#### Sender
* will be able to determine an error has occured if it recieves a SREJ (negative ack) with the sequence # of the missing frame or if a timer expires.

#### Reciver
* Will be able to determine an error has occured if the recieving packet checksum fials or ther is missing packet (out of order packets).
	* If an error occurs, the reciever will send a SREJ and buffer correct frames until it recieves the frame that was lost or corrupted again.

## Regarding buffering packets:
* This must a c style array.
* No 3rd party libraries or builtin data structures

## Window Flow Control:
### When you are sending data and your window is open
1. Send one data packet
2. check (non-blocking) for RRs/SREJ's
3. Process all available RRs/SREJ's
4. Repeat
**The server should process RRs/SREJs without blocking after every second.**
### When sending file data and your window closes on the server
* `Closed window`: means you have sent an entire windows' worth of data without recieving an RR
*
*
*
