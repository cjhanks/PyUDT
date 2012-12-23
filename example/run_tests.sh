#!/bin/bash 

set -e 

echo "---------------------------------------------"
echo "RUNNING udt4 tests!" 
echo "---------------------------------------------"

pushd udt4  

pushd mixed_python_c 
clear 
echo "........ TESTING LANGUAGE MIXING" 
./build.sh 
./run_tests.sh 
popd


pushd sendrecv_file 
clear 
echo "........ TESTING SEND/RECV FILE" 
./init.py 
popd 


pushd socket 
clear 
echo "........ TESTING DGRAM SENDING"
./init.py DGRAM 


clear 
echo "........ TESTING SOCK_STREAM SENDING"
./init.py NOT_DGRAM 
popd  

popd

# --- #
pushd pyudt 

pushd epoll 
clear 
echo "........ TESTING pyudt.Epoll()" 
./server.py 
wait 
popd 

pushd perfmon 
clear 
echo "........ TESTING pyudt.perfmon and socket"
./server.py 
popd 

pushd settings 
clear 
echo "........ TESTING pyudt.UdtSocket() and options"
./server.py 
popd 
popd  

clear 
echo "All tests passed!" 
