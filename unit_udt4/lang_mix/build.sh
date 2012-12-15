#!/bin/bash

LD_FLAGS="-ludt -lpthread"

if [ -e client ];
then
    rm client
    rm server 
fi 

g++ client.cc $LD_FLAGS -o client
g++ server.cc $LD_FLAGS -o server 
