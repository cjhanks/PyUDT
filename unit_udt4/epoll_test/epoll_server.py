#!/usr/bin/env python

from   threading import Thread 
import pyudt4 

class EpollThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.domain  = True
        self.__epoll = pyudt4.epoll() 


    def run(self):
        print('starting epoll thread') 
        
        while True:
            print('__iterate__')
        
            print( 
                self.__epoll.wait(False, False, 2000)
                )
            
    
    def add_rsock(self, sock):
        self.__epoll.add_usock(sock)


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
        print('in range...')
        sock, host = server.accept() 
        print(host) 
        #epoll.add_usock(
        #        server.accept()[0]
        #        )
        epoll.add_usock(sock)
    
    #epoll = pyudt4.epoll()  
    

if __name__ == '__main__':
    main({
            'host'  : '127.0.0.1',
            'port'  : 3004,
            'count' : 1
        })
