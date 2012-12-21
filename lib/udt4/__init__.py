
"""
:module: __init__.py 

--------------------------------------------------------------------------------
Namespace: udt4
--------------------------------------------------------------------------------
udt4
    |
    +-- .UDTSOCKET          : Raw UDTSOCKET reference 
    +-- .UDTepoll           : epoll implementation accepting UDTSOCKET instances
    +-- .UDTException       : Maps to UDTEXCEPTION (error_code, error_string) 
    +-- .TRACEINFO          : Direct structure map to UDT::TRACEINFO 
    |
    .pyudt
         |
         +-- .UdtSocket()   : Python style socket wrapper around UDTSOCKET  
         |
         +-- .Epoll()       : Epoll implementation to be used with UdtSocket()
--------------------------------------------------------------------------------

Versioning:


License:
UDT4 library and py-udt4 wrapper is licensed under BSD.
"""

__author__  = 'Christopher J. Hanks <develop@cjhanks.name>'
__date__    = '12/18/2012' 

import os 

import _udt4
from   _udt4 import *
import pyudt

__all__ = os._get_exports_list(_udt4) + \
         ['UdtSocket']
