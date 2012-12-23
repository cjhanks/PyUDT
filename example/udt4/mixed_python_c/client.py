#!/usr/bin/env python 

import socket as socklib
import udt4 
import struct 

udt4.startup() 

socket = udt4.socket(socklib.AF_INET, socklib.SOCK_STREAM, 0)

try:
    udt4.connect(socket, '127.0.0.1', 3001)
except RuntimeError as err:
    print(err)
    import sys
    sys.exit(0)

# recv message # 
msg_len = struct.unpack('I', udt4.recv(socket, 4))[0]
message = udt4.recv(socket, msg_len)

print(
    'received message: %s' % message
    )
assert len(message) == msg_len, 'invalid message received 0'

# send message #
message = 'no bottle found'
udt4.send(socket, struct.pack('I', len(message)), 4)
udt4.send(socket, message, len(message))

# cleanup #
udt4.close(socket)
udt4.cleanup()
