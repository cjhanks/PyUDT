#!/usr/bin/env python 

import udt4 as udt 
from   udt4 import pyudt 


def pre_opt_test(sock):
    """
    Iterate through all of the sock options and insure the values from 
    getsockopt come to match. 
    """
    tests = [ (udt.UDT_MSS   , 1400       ),
              (udt.UDT_SNDBUF, 2096416    ),
              (udt.UDT_RCVBUF, 2096416    ),
              (udt.UDP_SNDBUF, 1024 * 1024),
              (udt.UDP_RCVBUF, 1024 * 1024),
              (udt.UDT_MAXBW , 1024 * 54l ),
            ]

    for test in tests:
        sock.setsockopt(test[0], test[1])  
        assert sock.getsockopt(test[0]) == test[1], 'failed : %s' % str(test[1]) 
    
    sock.settimeout(3000, 40000) 


def post_opt_test(sock):
    """
    Set sock options valid for after connection 
    """
    tests = [ (udt.UDT_LINGER, (1, 60)    ),
              (udt.UDT_SNDSYN, True       ),
              (udt.UDT_RCVSYN, True       ),

            ]

    for test in tests:
        sock.setsockopt(test[0], test[1]) 
        assert sock.getsockopt(test[0]) == test[1], 'failed : %s' % str(test[1]) 
    


def main(argv):
    socket = pyudt.UdtSocket() 
    print('connect( (%s, %i) )' %(argv[1], int(argv[2])))
    
    pre_opt_test(socket)

    socket.connect( (argv[1], int(argv[2])) )
    print('connected...')
    post_opt_test(socket) 
    
    socket.perfmon() 

    for i in range(100):
        socket.send('a' * 1024) 
        sys.stderr.write('.') 
    
    for i in range(100):
        socket.recv(1024) 
        sys.stderr.write('.') 

     
    udt.dump_perfmon(socket.perfmon())     

    socket.close() 


if __name__ == '__main__':
    import sys 
    sys.exit(main(sys.argv))
    
