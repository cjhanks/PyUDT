#!/usr/bin/python

import os 

#host    = ''
host    = '127.0.0.1'
port    = 3444 

if os.fork():
    import server
    server.main(host, port) 
else:
    import client
    client.main(host, port) 

