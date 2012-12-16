""" @module pyudt4.py

Python-style sockets wrapping the (nearly) straight C++ --> Python 
implementation of module udt4.  All udt4 functions and constants are imported
into this module.

"""

import os
import socket as socklib
import udt4 
from   udt4 import *

__author__  = 'Christopher J. Hanks <develop@cjhanks.name>'
__date__    = '12/15/2012' 
__all__     = os._get_exports_list(udt4) + \
              [
                'UdtSocket'
              ]


class UdtSocket(object):
    """ @class UdtSocket
    A 'pythonic-wrapper' for the UDT4 library
    """
    def __init__(self, family = socklib.AF_INET, type = socklib.SOCK_STREAM,
                 protocol = 0, _sock = None):
        """
        """
        if _sock != None:
            self.__sock = _sock
        else:
            self.__sock = udt4.socket(family, type, protocol) 
    

    @property
    def family(self):
        return self.__sock.domain


    @property
    def proto(self):
        return self.__sock.protocol

    @property
    def type(self):
        return self.__sock.type


    def __del__(self):
        try:
            self.close()
        except Exception as err:
            pass


    def accept(self):
        """
        """
        pass 


    def bind(self):
        """
        """
        pass


    def close(self):
        """
        """
        pass


    def connect(self, address):
        """
        """
        udt4.connect(self.__sock, address[0], address[1])

    
    def connect_ex(self, address):
        """
        """
        try:
            self.connect(address)
        except Exception as err:
            return 1 # TODO: get exception error code 


    def dup(self):
        """
        """
        return UdtSocket(self.__sock.domain  , self.__sock.type, 
                         self.__sock.protocol, self.__sock)


    def fileno(self, udt_fileno = False):
        """
        @param  udt_fileno  Return udt fileno instead of udp?
        @type   bool() 
        
        Return the underlying fileno of the socket.  Since UDT is actually a 
        library wrapper for UDP the underlying socket has two id's.  The udt 
        fileno (which is more or less worthless) and the UDP fileno which is
        the default return.

        @return underlying fileno ::int() 
        """
        pass


    def getpeername(self):
        """
        This stills needs implementing in udt4 abstraction.

        @return tuple( str(hostname), int(port) )
        """
        pass


    def getsockname(self):
        """
        This stills needs implementing in udt4 abstraction.

        @return tuple( str(hostname), int(port) )
        """
        pass


    def getsockopt(self, option):
        """
        @param  option  Intrinsic class constant ie: pyudt.UDT_SNDBUF
        @type   option  int() 

        @return Value - return type is variable dependent on specified option,
                        can be bool(), int(), long(), or tuple() (for linger) 
        """
        # TODO: write documentation associating the return types with options
        return udt4.getsockopt(self.__sock, option)

   
    def gettimeout(self):
        """
        Always asserts an error since no such feature exists in UDT.
        """
        # TODO: rather than assert, probably return -1 
        assert False, 'gettimeout feature does not exist for UDT sockets '     \
                      'sockoptions SND_TIMEO and RCV_TIMEO are the preferred ' \
                      'method'


    def listen(self, backlog):
        """
        """
        udt4.listen(self.__sock, backlog)


    def makefile(self, mode = 'r', bufsize = -1):
        """
        It needs to be determined if this really makes sense for UDT, it should
        probably be added for interface compatibility, however the interface 
        natively supports a sendfile/recvfile option.
        """
        pass


    def sendall(self, data, flags = 0):
        """
        @param  data    String buffered data
        @type   data    str() 
        
        @param  flags   Ignored
        @type   flags   int() 

        This function exists for interface compatibility only, it's unclear in
        the Python socket documentation what benefit this has since the data 
        member's char* is gauranteed sequential.
        """
        return self.send(data, flags) 


    def setblocking(self, flag, flag_recv = None):
        """
        @param  flag        True or False
        @type   flag        bool() 

        @param  flag_recv   Differentiate between blocking modes on send/receive
                            and set receive block mode to this value.
        @type   flag_recv   bool() 

        All duplex settings in UDT make send and receive are differentiable
          ie: a socket can block send and non-block recv. 
          ie: a socket can sendtimeo = 3 and recvtimeo = -1 

        The default behaviour here is to tie both to flag.  If flag_recv is 
        defined, the flag parameter becomes flag_send.

        """
        if flag_recv != None:
            self.setsockopt(udt4.UDT_SNDSYN, flag     )
            self.setsockopt(udt4.UDT_RCVSYN, flag_recv)
        else:
            self.setsockopt(udt4.UDT_SNDSYN, flag)
            self.setsockopt(udt4.UDT_RCVSYN, flag)
    

    def setsockopt(self, option, value):
        """
        @param  option  Intrinsic class constant ie: pyudt4.UDT_RCVBUF
        @type   option  int() 

        @param  value   Variable type value.
        @type   value   bool(), int(), long(), or tuple() 

        Sets a socket option to the specified type.
        """
        udt4.setsockopt(self.__sock, option, value)

    
    def settimeout(self, timeo, timeo_recv = None):
        """
        @param  timeo       h
        @type   timeo       int() 

        @param  timeo_recv  h
        @type   timeo_recv  int() 
        
        Please read documentation in setblocking().
        """
        if timeo_recv != None:
            self.setsockopt(udt4.UDT_SNDTIMEO, timeo     )
            self.setsockopt(udt4.UDT_RCVTIMEO, timeo_recv)
        else:
            self.setsockopt(udt4.UDT_SNDTIMEO, timeo)
            self.setsockopt(udt4.UDT_RCVTIMEO, timeo)

    def shutdown(self, flags):
        """
        No such feature exists within UDT
        """
        pass
    

    def recv(self, size):
        pass 

    
    def recvfrom(self, size):
        """
        @see UdtSocket.recvmsg()
        """
        return self.recvmsg(size)


    def recvmsg(self, size):
        """
        For unreliable sending message sending 
        """
        pass


    def recvfile(self, fd):
        pass 


    def send(self, data, padding = None):
        pass

    
    def sendto(self, data, padding = None):
        """
        @see UdtSocket.sendmsg()
        """
        return self.sendmsg(data, padding) 


    def sendmsg(self, data, padding = None):
        pass 


    def sendfile(self, fd):
        pass 





if __name__ == '__main__':
    sock = UdtSocket()

    print(sock.family)
