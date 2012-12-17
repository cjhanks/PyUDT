#!/usr/bin/env python

from   threading import Thread 
import pyudt4 
import time 

class EpollThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.domain  = True
        self.__epoll = pyudt4.epoll() 
        self.__socks = []


    def run(self):
        from datetime import datetime 

        print('starting epoll thread') 
        
        start = datetime.utcnow() 
        while True:
            print((datetime.utcnow() - start).total_seconds())
        
            returns = self.__epoll.wait(True, True, 2000)
            print(returns) 

            for i in returns[0]:
                print(i)
                print(pyudt4.recv(i, 4096)[0:3])
   

    def add_usock(self, sock):
        print('add sock: %s' % sock)
        pyudt4.setsockopt(sock, pyudt4.UDT_RCVTIMEO, 8000)
        pyudt4.setsockopt(sock, pyudt4.UDT_SNDTIMEO, 8000)
        self.__epoll.add_usock(sock, 0x1 | 0x8)
        self.__socks.append(sock) 

    
    def check_socks(self):
        for sock in self.__socks:
            try:
                print(pyudt4.getsockopt(sock, pyudt4.UDT_STATE))
            except RuntimeError as err:
                print('Error: %s' % err)


def main(settings):
    """
    Initialize the server and wait on incoming sockets 
    """
    pyudt4.startup() 
    
    # initialize epoll thread 
    epoll = EpollThread() 
    epoll.start() 
    
    # init server in pyudt4 style
    print('init server')
    server = pyudt4.UdtSocket() 
    server.bind((settings['host'], settings['port']))
    server.listen(1024) 

    for i in range(0, settings['count']):
        sock, host = server.accept() 
        epoll.add_usock(sock)
    
    while True:
        time.sleep(4) 
        epoll.check_socks() 
    #epoll = pyudt4.epoll()  
    

if __name__ == '__main__':
    main({
            'host'  : '127.0.0.1',
            'port'  : 3004,
            'count' : 1
        })
