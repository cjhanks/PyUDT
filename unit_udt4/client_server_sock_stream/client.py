
import time
import struct 
import socket as socklib
import udt4 

print('__load__client')

client = None


def create_client(host, port):
    print('create_client(%s, %s)' % (host, port))

    global client 

    socket = udt4.socket(socklib.AF_INET, socklib.SOCK_STREAM,
                         socklib.AI_PASSIVE)
    
    #
    # set sock options 
    #
    opts = [ (udt4.UDT_SNDBUF   , 64),
             (udt4.UDT_RCVBUF   , 64)
             ]
    
    for opt in opts:
        udt4.setsockopt(socket, opt[0], opt[1]) 
   
    print('connecting client')
    try:
        udt4.connect(socket, host, port)
    except Exception as err:
        print('Exception: %s' % err)
        return 0
   
    client = socket 

    return True


def test0():
    print('client: test0')

    size = struct.unpack('I', udt4.recv(client, 4))[0]
    msg  = udt4.recv(client, size)

    assert size == len(msg), 'message length does not match' 

    
def test1():
    print('client: test1')

    size = struct.unpack('I', udt4.recv(client, 4))[0]
    msg  = udt4.recv(client, size) 

    print(
        'test1: %i:%i %s' % (size, len(msg), msg)
            )


def main(host, port):
    udt4.startup() 
    
    time.sleep(1) # just to wait for server
    
    if not create_client(host, port):
        print('failed to create client')
        return 1

    test0()
    test1()

    udt4.close(client)

