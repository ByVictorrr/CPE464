
// Hugh Smith April 2017
// Network code to support TCP client server connections

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname6.h"
#include "NetworkNodes.hpp"
#include "safe_sys_calls.h"



int selectCall(int socketNumber, int seconds, int microseconds, int timeIsNotNull)
{
	// Returns 1 if socket is ready, 0 if socket is not ready  
	// set timeIsNotNull = TIME_IS_NOT_NULL when providing a time value
	int numReady = 0;
	fd_set fileDescriptorSet;  // the file descriptor set 
	struct timeval timeout;
	struct timeval * timeoutPtr;   // needed for the time = NULL case
	
	
	// setup fileDescriptorSet (socket to select on)
	  FD_ZERO(&fileDescriptorSet);
	  FD_SET(socketNumber, &fileDescriptorSet);
	
	// Time can be NULL, 0 or a seconds/microseconds 
	if (timeIsNotNull == TIME_IS_NOT_NULL)
	{
		timeout.tv_sec = seconds;  
		timeout.tv_usec = microseconds; 
		timeoutPtr = &timeout;
    } else
    {
		timeoutPtr = NULL;  /* block forever - until input */
    }

	numReady = safe_select(socketNumber + 1, &fileDescriptorSet, NULL, NULL, timeoutPtr);
	// We have to wait for a response after we send something
  
	// will only by 1 or 0
    return numReady;
}