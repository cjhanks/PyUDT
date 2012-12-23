#!/bin/bash 

set -e

function pyserver_cclient {
    echo "Server:   Python"
    echo "Client:   C"

    python server.py &
    sleep 1
    ./client 
}


function pyserver_pyclient {
    echo "Server:   Python"
    echo "Client:   Python"
    
    python server.py &
    sleep 1
    python client.py 
}


function cserver_pyclient {
    echo "Server:   C"
    echo "Client:   Python"

    ./server & 
    sleep 1 
    python client.py 
}


function cserver_cclient {
    echo "Server:   C"
    echo "Client:   C"

    ./server &
    sleep 1 
    ./client 
}

echo "-----------------------"
pyserver_cclient 
echo "-----------------------"
pyserver_pyclient
echo "-----------------------"
cserver_pyclient
echo "-----------------------"
cserver_cclient
