#!/usr/bin/env python

from   threading import Thread 
import udt4
import udt4.pyudt as pyudt
import time 

class EpollThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.domain  = True
        self.__epoll = udt4.UDTepoll() 
        self.__socks = []
        self.__cont  = True
    
    
    def run(self):
        from datetime import datetime 

        print('starting epoll thread') 
        
        start = datetime.utcnow() 
        while True:
            print((datetime.utcnow() - start).total_seconds())
        
            returns = self.__epoll.wait(True, True, 2000)
            print(returns) 

            for i in returns[0]:
                print(udt4.recv(i, 4096)[0:3])
            
            if not self.__cont:
                break 
   

    def add_usock(self, sock):
        udt4.setsockopt(sock, udt4.UDT_RCVTIMEO, 8000)
        udt4.setsockopt(sock, udt4.UDT_SNDTIMEO, 8000)
        self.__epoll.add_usock(sock, 0x1 | 0x8)
        self.__socks.append(sock) 

    
    def check_socks(self):
        for sock in self.__socks:
            print(sock)
            sock.debug() 
            try:
                print(udt4.getsockopt(sock, udt4.UDT_STATE))
            except udt4.UDTException as err:
                print('Error: %s' % err)
                self.__cont = False
                return False 

        return True
        


def main(settings):
    """
    Initialize the server and wait on incoming sockets 
    """
    udt4.startup() 
    
    # initialize epoll thread 
    epoll = EpollThread() 
    epoll.start() 
    
    # init server in udt4 style
    print('init server')
    server = pyudt.UdtSocket() 
    server.bind((settings['host'], settings['port']))
    server.listen(1024) 

    for i in range(0, settings['count']):
        sock, host = server.accept() 
        epoll.add_usock(sock)
    
    while epoll.check_socks():
        time.sleep(4) 
    

if __name__ == '__main__':
    main({
            'host'  : '127.0.0.1',
            'port'  : 3004,
            'count' : 1
        })
