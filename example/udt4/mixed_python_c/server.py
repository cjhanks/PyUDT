#!/usr/bin/env python 

import socket as socklib
import struct
import udt4 

socket = udt4.socket(socklib.AF_INET, socklib.SOCK_STREAM, 0)

udt4.bind(socket, '127.0.0.1', 3001)
udt4.listen(socket, 10)

sock, host = udt4.accept(socket)

# send message #
message = 'message in a bottle'

udt4.send(sock, struct.pack('I', len(message)), 4)
udt4.send(sock, message, len(message))


# recv message #
msg_len = struct.unpack('I', udt4.recv(sock, 4))[0]

message = udt4.recv(sock, msg_len) 

print(
    'received message: %s' % message
    )

assert len(message) == msg_len, 'invalid message'
