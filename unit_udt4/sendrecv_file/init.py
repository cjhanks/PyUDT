#!/usr/bin/python

import os 

#host    = ''
settings = {
        'host'  :   '127.0.0.1',
        'port'  :   3444
        }

if os.fork():
    import server
    server.main(settings) 
else:
    import client
    client.main(settings) 

