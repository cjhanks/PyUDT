#!/usr/bin/python 

import time 

import socket 
import udt4 

udt4.startup()
print(socket.AF_INET)
sock0 = udt4.socket(socket.AF_INET, socket.SOCK_STREAM, socket.AI_PASSIVE)
sock1 = udt4.socket(socket.AF_INET, socket.SOCK_STREAM, socket.AI_PASSIVE)

print(sock0)
print(sock1)

print(udt4.bind(sock0, '127.0.0.1', 4001))
print('listen')
udt4.listen(sock0, 40)
udt4.accept(sock0)
print('listen')

udt4.cleanup() 


time.sleep(100) 
