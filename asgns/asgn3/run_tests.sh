#!/bin/bash

# ./rcopy <from-filename> <to-filename> <buffer-size> <error-percent> <window-size> <remote-machine> <remote-port> 
# ./server <error-percent> [remote-port]

: '-----Test 0------
  Given : window-size = 5, error-rate = 0 (on both) 
'
FROM_DIR="'$pwd'/tests/from_tests"
TO_DIR="'$pwd'/tests/to_tests"
PORT=46454



RCOPY_FROM_FILE=
RCOPY_TO_FILE=
RCOPY_WINDOW_SIZE=
RCOPY_ERR_PER= 
RCOPY_BUFFER_SIZE=



SERVER_ERR_PER=0

function test0(){
    RCOPY_FROM_FILE=$1 #input
    RCOPY_TO_FILE=$2 #input
    RCOPY_WINDOW_SIZE=5  #given
    RCOPY_ERR_PER=0 #given
    RCOPY_BUFFER_SIZE=10
    SERVER_ERR_PER=0
    echo "===test0 (ws=5, error-rate=0)==="
    ./server $SERVER_ERR_PER $PORT
    ./rcopy $RCOPY_FROM_FILE $RCOPY_TO_FILE $BUFFER_SIZE $RCOPY_ERR_PER $RCOPY_WINDOW_SIZE localhost $PORT
}

test0

#1. File small, window=10, buffer=1000, errorRate=.2^2
function test1(){
    echo "===test1 (bs=1000, error-rate=.2^2, ws=10)==="
}


