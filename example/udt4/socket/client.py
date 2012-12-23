
import time
import struct 
import socket as socklib
import udt4 

print('__load__client')

client = None
mode   = None

def create_client(host, port):
    print('create_client(%s, %s)' % (host, port))

    global client 

    socket = udt4.socket(
            socklib.AF_INET, 
           (socklib.SOCK_STREAM, socklib.SOCK_DGRAM)[mode == 'DGRAM'], 
            socklib.AI_PASSIVE
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


def test0():
    print('client: test0')
    
    if mode == 'DGRAM':
        dat = udt4.recvmsg(client, 4) 
        msg = udt4.recvmsg(client, struct.unpack('I', dat)[0])
    else:
        dat = udt4.recv(client, 4)
        msg = udt4.recv(client, struct.unpack('I', dat)[0])
    
    
    
def test1():
    print('client: test1')
    
    if mode == 'DGRAM':
        dat = udt4.recvmsg(client, 4)
        msg = udt4.recvmsg(client, struct.unpack('I', dat)[0])
    else:
        dat = udt4.recv(client, 4)
        msg = udt4.recv(client, struct.unpack('I', dat)[0])
        
    print(
        'test1: %i %s' % (len(msg), msg)
            )


def main(settings):
    global mode 
    mode = settings['mode'] 

    udt4.startup() 
    
    #time.sleep(1) # just to wait for server
    
    if not create_client(settings['host'], settings['port']):
        print('failed to create client')
        return 1
    
    perf = udt4.perfmon(client) 
    print(perf)
    print(perf.pktSentTotal)
    print(perf.pktRecvTotal)
    print(perf.usPktSndPeriod)
    print(perf.msRTT)
    test0()
    test1()

    udt4.close(client)

