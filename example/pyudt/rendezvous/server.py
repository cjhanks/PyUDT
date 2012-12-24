#!/usr/bin/python 

import sys 
import udt4 as udt 
from   udt4 import pyudt 


def main(argv):
    serv = pyudt.UdtSocket()
    serv.bind(('127.0.0.1', 4032))
    serv.listen(100) 

    c0, c0_host = serv.accept()
    print(c0_host)
    
    try:
        print c0.recv(len('words'))
    except udt.UDTException as err:
        print(err) 
    #c1, c1_host = serv.accept()
    #print(c1_host) 
    
    print('continue on')


if __name__ == '__main__':
    sys.exit(main(sys.argv))
