FROM_DIR=tests/from_files
TO_DIR=tests/to_files



for


## works
#make && sh testRandError.sh 46552 /home/victord/CPE464/asgns/asgn3/tests/from_files/test1 .80 100 
## sh testRandError.sh 46552 /home/victord/CPE464/asgns/asgn3/tests/from_files/test1 .1 100 
## does not work
## sh testRandError.sh 46552 /home/victord/CPE464/asgns/asgn3/tests/from_files/test1 .9 100 


test0_rcopy: rcopy
											# <buff-size> <err-per> <wind-size>
	./rcopy $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) $(BUFF_SIZE) $(RCOPY_ERR_PER) $(WIND_SIZE) $(REMOTE_MACHINE) $(PORT)
	diff $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) 

test0_server: server
	./server $(SERVER_ERR_PER) $(PORT)

debug0_rcopy: rcopy
	gdb --args ./rcopy $(FROM_DIR)/$(FROM_TEST) $(TO_DIR)/$(TO_TEST) $(BUFF_SIZE) $(RCOPY_ERR_PER) $(WIND_SIZE) $(REMOTE_MACHINE) $(PORT)

debug0_server: server
	gdb --args ./server $(SERVER_ERR_PER) $(PORT)

