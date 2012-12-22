#!/usr/bin/env python 

""" 
:module udtserver 
"""

import struct 
import udt4 as     udt 
from   udt4 import pyudt 
import socket as socklib
from   subprocess import Popen  


def configure_epoll(udt_children, sys_children):
    epoll = pyudt.Epoll() 

    for child in udt_children:
        epoll.add_usock(child, udt.UDT_EPOLL_IN) 

    for child in sys_children:
        epoll.add_ssock(child, udt.UDT_EPOLL_IN) 
    
    return epoll 


def acquire_udt_children(host, port, count):
    udt_serv = pyudt.UdtSocket() 
    udt_serv.bind( (host, port) ) 
    udt_serv.listen(100) 
    
    children = list() 

    for i in xrange(count):
        Popen('./client.py %(host)s %(protocol)s %(port)i ' % {
                'host' : host, 'port' : port, 'protocol' : 'udt' },
              shell = True 
              )

        socket, host = udt_serv.accept()
        
        children.append(socket)
    
    return children 


def acquire_tcp_children(host, port, count):
    tcp_serv = socklib.socket() 
    tcp_serv.bind( (host, port) )
    tcp_serv.listen(100) 

    children = list() 

    for i in xrange(count):
        Popen('./client.py %(host)s %(protocol)s %(port)i ' % {
                'host' : host, 'port' : port, 'protocol' : 'tcp' },
              shell = True 
              )

        socket, hot = tcp_serv.accept() 
        
        children.append(socket) 

    return children 


def handle_socket_set(epoll, sock_set):
    """
    Ideally this function should work for both UdtSocket type and socket.socket 
    type.
    
    standard:
    4   byte - version
    8   byte - length 
    k   byte - message 
    """
    for sock in sock_set:
        try:
            version = struct.unpack('i', sock.recv(4)) 
        except Exception as err:
            print('socket no longer good') 
            epoll.remove_ssock(sock)
            continue 

        msg_len = struct.unpack('l', sock.recv(8))[0]
        msg     = sock.recv(msg_len) 
        
        print msg[0:7]


def main():
    """
    """

    udt_children = acquire_udt_children('127.0.0.1', 4001, 10)
    tcp_children = acquire_tcp_children('127.0.0.1', 4002, 10)
    
    epoll = configure_epoll(udt_children, tcp_children) 
    
    while True:
        sets = epoll.wait(True, False, 1000, True, False)
        
        print(sets) 
        for set in sets: 
            handle_socket_set(epoll, set) 



if __name__ == '__main__':
    from sys import exit
    exit(main()) 

