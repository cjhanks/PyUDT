#!/usr/bin/env python 

import sys 

import time 
import struct 
import udt4 as udt
from   udt4 import pyudt 
import socket as socklib 

version = int(0)  

def tcp_main(argv):
    sock = socklib.socket() 
    sock.connect(
            (sys.argv[1], int(sys.argv[3]))
            ) 
   
    for x in range(3):
        message = 'hey ' * 30

        sock.send(struct.pack('i',  version))
        sock.send(struct.pack('l', len(message))) 
        sock.send(message) 
        
        time.sleep(x) 
    
    time.sleep(5) 

    sock.close() 



def udt_main(argv):
    sock = pyudt.UdtSocket() 
    sock.connect(
            (sys.argv[1], int(sys.argv[3]))
            ) 
    
    for x in range(3):
        message = 'you ' * 30

        sock.send(struct.pack('i',  version))
        sock.send(struct.pack('l', len(message))) 
        sock.send(message) 
        
        time.sleep(x) 
        
    time.sleep(5) 

    sock.close() 


if __name__ == '__main__':
    import sys 

    if sys.argv[2] in ('tcp'):
        sys.exit(tcp_main(sys.argv)) 
    else:
        sys.exit(udt_main(sys.argv)) 

