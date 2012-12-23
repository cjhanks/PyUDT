#!/usr/bin/python

import os 
import sys 

if len(sys.argv) != (1 + 1):
    raise RuntimeError(
            'init.py should be ran with one arg DGRAM or SOCK_STREAM'
            )

#host    = ''
settings = {
        'host'  :   '127.0.0.1',
        'port'  :   3444,
        'mode'  :   sys.argv[1]  
        }


if os.fork():
    import server
    server.main(settings) 
else:
    import client
    client.main(settings) 

