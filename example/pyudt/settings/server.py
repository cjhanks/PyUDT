#!/usr/bin/env python 

import sys 
import udt4 as udt 
from   udt4 import pyudt 
from   subprocess import Popen 




def main(argv):
    Popen('./client.py %(host)s %(port)i' % { 
          'host' : '127.0.0.1', 'port': 3001 },
          shell = True) 

    server = pyudt.UdtSocket() 
    server.bind( ('127.0.0.1', 3001) )
    server.listen(100) 
    
    client, host = server.accept()
    print('accepted:') 
    
    for i in range(100):
        client.recv(1024) 
    
    for i in range(100):
        client.send('a' * 1024) 
    
    client.close() 
    server.close() 

if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
