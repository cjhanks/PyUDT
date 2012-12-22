#!/usr/bin/env python 

import sys 
import udt4 as udt 
from   udt4 import pyudt 
from   subprocess import Popen 
from   threading import Thread 


class Handler(Thread):
    def __init__(self, lmbda):
        Thread.__init__(self) 
        self.__lmbda = lmbda 

    def run(self):
        self.__lmbda() 


def send0(sock):
    # send 128 mb of data 
    for i in range(128):
        sys.stderr.write('.')
        for j in range(1024):
            sock.send('a' * 1024)
        

def recv1(sock):
    # recv 128 mb of data 
    for i in range(128):
        sys.stderr.write('.')
        for j in range(1024):
            sock.recv(1024) 
    

def main(argv):
    Popen('./client.py %(host)s %(port)i' % { 
          'host' : '127.0.0.1', 'port': 3001 },
          shell = True) 

    server = pyudt.UdtSocket() 
    server.bind( ('127.0.0.1', 3001) )
    server.listen(100) 
    
    client, host = server.accept()
    
    h0 = Handler(lambda: send0(client))
    h0.start() 
    h1 = Handler(lambda: recv1(client)) 
    h1.start() 

    h0.join()
    h1.join() 
    
    client.close() 
    server.close() 

if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
