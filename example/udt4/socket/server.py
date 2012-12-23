
import socket as socklib
import struct 
import udt4 

print('__load__server')

server = None
mode   = None 

def create_server(host, port):
    print('create_server(%s, %s)' % (host, port))
    
    global server 
    
    socket = udt4.socket(
            socklib.AF_INET, 
           (socklib.SOCK_STREAM, socklib.SOCK_DGRAM)[mode == 'DGRAM'], 
            socklib.AI_PASSIVE
            ) 
    
    print(
            ('socklib.SOCK_STREAM', 'socklib.SOCK_DGRAM')[mode == 'DGRAM']
            )
    
    #
    # set sock options 
    #
    opts = [ (udt4.UDT_SNDBUF   , 64 * 1024),
             (udt4.UDT_RCVBUF   , 64 * 1024),
             (udt4.UDT_REUSEADDR, True     )
             ]

    for opt in opts:
        udt4.setsockopt(socket, opt[0], opt[1])
    
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


def test0(sock):
    """ Send a basic message
    """
    print('server: test0')

    message = 'message in a bottle'
    
    if mode == 'DGRAM':
        udt4.sendmsg(sock, struct.pack('I', len(message)), 4) 
        udt4.sendmsg(sock, message, len(message))
    else:
        udt4.send(sock, struct.pack('I', len(message)), 4) 
        udt4.send(sock, message, len(message))
   

def test1(sock):
    """ Send a basic message on a padded size, this is useful if you might be 
    sending serialized data where the receiver already has a fixed buffer 
    prepared
    """
    print('server: test1')

    pad = 64
    msg = 'words are only words except when they are not'
    
    print(pad) 
    if mode == 'DGRAM':
        udt4.sendmsg(sock, struct.pack('I', pad), 4)
        udt4.sendmsg(sock, msg, pad)
    else:
        udt4.send(sock, struct.pack('I', pad), 4)
        udt4.send(sock, msg, pad)


def test2(sock):
    print('server: test2')


def main(settings):
    global mode 
    mode = settings['mode'] 
    
    udt4.startup() 

    if not create_server(settings['host'], settings['port']):
        print('failed to create_server')
        return 0
    else:
        print('server socket created successfully')

    
    sock = accept_client() 
    
    test0(sock)
    test1(sock)
    test2(sock)
    
    udt4.close(sock) 
    udt4.close(server) 

    udt4.cleanup() 
