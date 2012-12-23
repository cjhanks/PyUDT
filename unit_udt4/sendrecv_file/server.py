
import socket as socklib
import struct 
import udt4 

print('__load__server')

server = None

def create_server(host, port):
    print('create_server(%s, %s)' % (host, port))
    
    global server 
    
    socket = udt4.socket(
            socklib.AF_INET, socklib.SOCK_STREAM, socklib.AI_PASSIVE
            ) 
    
    #
    # set sock options 
    #
    opts = [ (udt4.UDT_SNDBUF   , 64 * 1024),
             (udt4.UDT_RCVBUF   , 64 * 1024),
             (udt4.UDT_REUSEADDR, True     )
             ]

    #for opt in opts:
    #    udt4.setsockopt(socket, opt[0], opt[1])
    
    udt4.bind(socket, host, port)
    udt4.listen(socket, 10)
    
    server = socket

    return True 


def accept_client():
    print('waiting on client')
    sock, host = udt4.accept(server)
    
    print(
        'client found: %s' % host
        )
    
    return sock


def recv_file(sock):
    odat = open('odat', 'wb')

    udt4.recvfile(sock, odat.name, 0, 33554432)


def main(settings):
    udt4.startup() 

    if not create_server(settings['host'], settings['port']):
        print('failed to create_server')
        return 0
    else:
        print('server socket created successfully')
    
    sock = accept_client() 
    
    recv_file(sock)

    udt4.close(sock) 
    udt4.close(server) 

    udt4.cleanup() 
