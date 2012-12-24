#!/usr/bin/env python 

import udt4 as udt 
from   udt4 import pyudt 

def main(argv):
    socket = pyudt.UdtSocket() 
    socket.connect(('127.0.0.1', 4032)) 
    
    import time 
    time.sleep(190) 
    socket.send('words')

if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
    
