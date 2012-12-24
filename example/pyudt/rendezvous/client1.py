#!/usr/bin/env python 

import udt4 as udt 
from   udt4 import pyudt 

def main(argv):
    socket = pyudt.UdtSocket() 
    socket.connect(('127.0.0.1', 4032)) 


if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
    
