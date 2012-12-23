#!/usr/bin/env python 

import udt4 as udt 
from   udt4 import pyudt 
from   threading import Thread 

class Handler(Thread):
    def __init__(self, lmbda):
        Thread.__init__(self) 
        self.__lmbda = lmbda 

    def run(self):
        self.__lmbda() 


def recv0(sock):
    for i in range(128):
        for j in range(1024):
            sock.recv(1024) 

def send1(sock):
    # send 128 mb of data 
    for i in range(128):
        for j in range(1024):
            sock.send('a' * 1024) 


def main(argv):
    socket = pyudt.UdtSocket() 
    print('connect( (%s, %i) )' %(argv[1], int(argv[2])))
    socket.connect( (argv[1], int(argv[2])) )
    
    print('connected')
    udt.dump_perfmon(socket.perfmon())  
    
    h0 = Handler(lambda: recv0(socket))
    h0.start() 
    h1 = Handler(lambda: send1(socket)) 
    h1.start() 

    h0.join()
    h1.join() 
    

    udt.dump_perfmon(socket.perfmon())  
    socket.close() 


if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
    
