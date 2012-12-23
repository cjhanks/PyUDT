
import time
import struct 
import socket as socklib
import udt4 

print('__load__client')

client = None

def create_client(host, port):
    print('create_client(%s, %s)' % (host, port))

    global client 

    socket = udt4.socket(
            socklib.AF_INET, socklib.SOCK_STREAM, socklib.AI_PASSIVE
            ) 
    
    #
    # set sock options 
    #
    opts = [ (udt4.UDT_SNDBUF   , 64),
             (udt4.UDT_RCVBUF   , 64)
             ]
    
    #for opt in opts:
    #    udt4.setsockopt(socket, opt[0], opt[1]) 
   
    print('connecting client')
    try:
        udt4.connect(socket, host, port)
    except Exception as err:
        print('Exception: %s' % err)
        return 0
   
    client = socket 

    return True


def init_file():
    # initialize a file # 
    idat = open('/dev/urandom', 'r')
    odat = open('dat', 'wb') 

    odat.write(idat.read(32 * 1024 * 1024)) 

    return open('dat', 'r')


def send_file(fp):
    import os 
    udt4.sendfile(client, fp.name, 0, os.stat(fp.name).st_size)


def main(settings):
    udt4.startup() 
    
    #time.sleep(1) # just to wait for server
    
    if not create_client(settings['host'], settings['port']):
        print('failed to create client')
        return 1
    
    send_file(init_file())
    
    import time
    time.sleep(10)


    udt4.close(client)

