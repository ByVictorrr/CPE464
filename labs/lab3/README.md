Programming (You may work together in your team on design of the program changes specified below (submittal not required) but make the program changes and submittal  individually. Program submittal by Monday, October 5 at 11:59 PM):

Compile the myclient/myserver code provided and verify that the client talks with the server.
Modify the Makefile to produce executables called cclient and server

Modify the code:
Modify both the client and server so that they send/recv messages in the following format:
Two-byte packet length in network order, then a null terminated text message

Length (2 bytes)
Null terminated text message

The packet length is for the entire packet (2 bytes of length+message)
You are creating a simple application level PDU (header + data)
All messages must be sent in one send() call 
All messages must be received in two recv() calls using the MSG_WAITALL 

Modify the Server
Make the server receive messages (loop) from the client until the client sends the message with text exit or the client terminates (^C).
After the client ends (either by sending exit or client terminates), server should go back to process (accept()) a new client.  The server does not end until its killed (^C).
Modify the server to handle the case where the client ends without sending an exit but instead terminates (e.g. ^C).
Modify the output of the server to include message length and message:
e.g.: Recv Len 8, Header Len: 8, Message: hello        
 (note the length is 8 because the NULL was also sent as part of the message)


Modify the Client
Make the client loop until the exit is entered.  Client should send the exit message before terminating.


Handin all of your code including Makefile, client and server code and the network code (and any other code we need to compile these two programs).

Handin using the lab3_sockets directory:  handin csc-cpe464 lab3_sockets server.c (and more)

