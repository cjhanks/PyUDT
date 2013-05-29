""" 
:module: pyudt4.py

Python-style sockets wrapping the (nearly) straight C++ --> Python 
implementation of module udt4.  All udt4 functions and constants are imported
into this module.

License:
UDT4 library and py-udt4 wrapper is licensed under BSD.

"""

import socket as socklib
import udt4 

__author__  = 'Christopher J. Hanks <develop@cjhanks.name>'
__date__    = '12/15/2012' 
__version__ = (0, 6) 

# assert py-udt4 version is in sync with udt4.pyudt version
# please read __init__.py for explanation of versiong schema  
if not __version__ == udt4.pyudt4_version():
    version = udt4.pyudt4_version() 

    raise ImportError(
        'udt4.pyudt version expects version %i.%i and [_udt4.so] is %i.%i' % (
            __version__[0], __version__[1], version[0], version[1]
            )
        )

class UdtSocket(object):
    """ 
    :class UdtSocket:

    A 'pythonic-wrapper' for the UDT4 library
    """

    def __init__(self, family = socklib.AF_INET, type = socklib.SOCK_STREAM,
                 protocol = 0, _sock = None):
        """
        :param  family:     UDT requires this family to be AF_INET 
        :type   family:     int() 

        :param  type:       SOCK_STREAM or SOCK_DGRAM 
        :type   type:       int() 
        
        :param  protocol:   AI_PASSIVE or int(0) are tested working.
        :type   protocol:   int() 
        
        :param  _sock:      Internally used to implement a dup()  
        :type   _sock:      UdtSocket() 

        """
        if _sock != None:
            self.__sock = _sock
        else:
            self.__sock = udt4.socket(family, type, protocol) 
    
    
    @property
    def family(self):
        """ 
        :return: socket domain
        """
        return self.__sock.domain


    @property
    def proto(self):
        """
        :return: socket protocol
        """
        return self.__sock.protocol


    @property
    def type(self):
        """
        :return: socket type
        """
        return self.__sock.type

    
    @property
    def UDTSOCKET(self):
        """
        :return: the underlying udtsocket primitive
        """
        return self.__sock 


    def __del__(self):
        """
        UDT typically has a very large write and read buffer due to its UDP 
        aknowledgment strategy (this is only a guess).  As a result data will
        appear as written while it has significant data in wait. 

        Resultantly __del__ may appear to 'hang', this is the defined correct
        behaviour.
        """
        pass
    

    def accept(self):
        """
        Blocking call.  Currently this call will block permanently and 
        EscapeExceptions will not break its execution (unforunately)

        @return tuple( UdtSocket(sock), str(host) )
        """
        accept = udt4.accept(self.__sock) 

        return UdtSocket(_sock = accept[0]), accept[1] 


    def bind(self, arg):
        """
        Binds the UDT socket to address and specific port OR binds 
	UDT directly on an existing UDP socket.

        :param  arg:    device ip port pair or an existing UDP socket  
        :type   arg:    tuple( str(), int() ) or a UDP socket.socket
        """
	if isinstance(arg,socklib.socket):
		udt4.bind_to_udp(self.__sock,arg.fileno())
	else:
        	udt4.bind(self.__sock, str(arg[0]), int(arg[1]))
        

    def close(self):
        """
        Closes a socket connection, please @see UdtSocket.__del__ for expected
        behaviour explanation.
        """
        udt4.close(self.__sock)  


    def connect(self, address):
        """
        Connects local socket to specific host port pair

        :param  address:    Host:Port pair
        :type   address:    tuple( str(), int() ) 
        """
        udt4.connect(self.__sock, address[0], address[1])

    
    def connect_ex(self, address):
        """
        Like UdtSocket.connect() except any Errors are caught and returned as
        error codes.

        :return: UDT exception 
        :type  : int() 
        """
        try:
            self.connect(address)
        except udt4.UDTException as err:
            return err[0] 
        
        return 0 


    def dup(self):
        """
        Duplicates the socket and place it in a second socket.  There is no 
        'true' dup() in UDT.  The way the library works behind the scene it will
        work as expected anyways.
        """
        return UdtSocket(self.__sock.domain  , self.__sock.type, 
                         self.__sock.protocol, self.__sock)


    def fileno(self, udt_fileno = True):
        """
        Return the associated UDTSOCKET instance.  
        """
        if udt_fileno:
            return self.__sock.UDTSOCKET
        else:
            assert 1 == 2, 'UDP .fileno() not implemented yet'


    def getpeername(self):
        """
        This stills needs implementing in udt4 abstraction.

        @return tuple( str(hostname), int(port) )
        """
        return udt4.getpeername(self.__sock) 


    def getsockname(self):
        """
        This stills needs implementing in udt4 abstraction.

        @return tuple( str(hostname), int(port) )
        """
        return udt4.getsockname(self.__sock)  


    def getsockopt(self, option):
        """
        :param  option  Intrinsic class constant ie: pyudt.UDT_SNDBUF
        :type   option  int() 

        :return: Value - return type is variable dependent on specified option,
                         can be bool(), int(), long(), or tuple() (for linger) 
        """
        # TODO: write documentation associating the return types with options
        return udt4.getsockopt(self.__sock, option)
    
    
    def getsockstate(self):
        """
        :return: Current socket state 
        :type  : int() 
        """
        return udt4.getsockstate(self.__sock) 

   
    def gettimeout(self):
        """
        :return:    The send-timeout and recv-timeout 
        :type  :    tuple( int(), int() )
        """
        return tuple(self.getsockopt(udt4.UDT_SNDTIMEO),
                     self.getsockopt(udt4.UDT_RCVTIMEO))


    def listen(self, backlog):
        """
        Set socket listen count 

        :param  backlog Backlog count
        :type   backlog int() 
        """
        udt4.listen(self.__sock, backlog)


    def makefile(self, mode = 'r', bufsize = -1):
        """
        It needs to be determined if this really makes sense for UDT, it should
        probably be added for interface compatibility, however the interface 
        natively supports a sendfile/recvfile option.
        """
        pass
    

    def perfmon(self, clear = True):
        """
        Garther a current performance status of socket. 

        :param clear:   Clear history on checking 
        :type  clear:   bool() 

        :return:    udt4.TRACEINFO 
        """
        return udt4.perfmon(self.__sock, clear) 


    def sendall(self, data, flags = 0):
        """
        :param  data    String buffered data
        :type   data    str() 
        
        :param  flags   Ignored
        :type   flags   int() 

        This function exists for interface compatibility only, it's unclear in
        the Python socket documentation what benefit this has since the data 
        member's char* is gauranteed sequential.
        """
        return self.send(data, flags) 


    def setblocking(self, flag, flag_recv = None):
        """
        :param  flag        True or False
        :type   flag        bool() 

        :param  flag_recv   Differentiate between blocking modes on send/receive
                            and set receive block mode to this value.
        :type   flag_recv   bool() 

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
        :param  option  Intrinsic class constant ie: pyudt4.UDT_RCVBUF
        :type   option  int() 

        :param  value   Variable type value.
        :type   value   bool(), int(), long(), or tuple() 

        Sets a socket option to the specified type.
        """
        udt4.setsockopt(self.__sock, option, value)

    
    def settimeout(self, timeo, timeo_recv = None):
        """
        :param  timeo       h
        :type   timeo       int() 

        :param  timeo_recv  h
        :type   timeo_recv  int() 
        
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
        print(
            'WARNING: shutdown() does not exist in UDT4'
            )
    

    def read(self, size):
        """
        Wraps .recv() 
        """
        return self.recv(size) 


    def recv(self, size):
        """
        Receive data from socket.

        @return str(returned_data) 
        """
        return udt4.recv(self.__sock, size)
        
    
    def recvfrom(self, size):
        """
        @see UdtSocket.recvmsg()
        """
        return self.recvmsg(size)


    def recvmsg(self, size):
        """
        For unreliable sending message receiving

        @return str(returned_data)
        """
        return udt4.recvmsg(self.__sock, size)  


    def recvfile(self, fd):
        # method signature wrong
        """
        Receive a file to the provided fd 
        
        @return int(bytes_written)
        """
        # TODO: This needs fixing inside the udt4 library 
        if isinstance(basestring, fd):
            return udt4.recvfile(self.__sock, fd)
        else:
            return udt4.recvfile(self.__sock, fd.name)

    
    def write(self, data):
        """
        Wraps .send() 
        """
        return self.send(data) 


    def send(self, data, padding = None):
        """
        In non-blocking mode this will raise an exception, in blocking 
        mode (default) the call will wait on freed buffer space.

        :param  data    Data to be written
        :type   data    str() 

        :param  padding Set a specified size the data should be written onto.
                        eg: len(data) => 45 padding = 64, data would be sent as
                        |45 bytes data|19 byte bzero| 

        :type   padding int()
        """
        if None == padding:
            return udt4.send(self.__sock, data, len(data))
        else:
            return udt4.send(self.__sock, data, padding  )

    
    def sendto(self, data, padding = None):
        """
        :see: UdtSocket.sendmsg()
        """
        return self.sendmsg(data, padding) 


    def sendmsg(self, data, padding = None, ttl = -1, inorder = False):
        """
        In non-blocking mode this will raise an exception, in blocking 
        mode (default) the call will wait on freed buffer space.

        :param  data    Data to be written
        :type   data    str() 

        :param  padding Set a specified size the data should be written onto.
                        eg: len(data) => 45 padding = 64, data would be sent as
                        |45 bytes data|19 byte bzero| 
        :type   padding int()

        :param  ttl     Time to live for the message in ms : -1 is infinite 
                        ie: garuanteed arrival
        :type   ttl     int()

        :param  inorder Guarantee UDP messages will be received in the order 
                        they were sent.
        :type   inorder bool() 
        """ 
        if None == padding:
            return udt4.sendmsg(self.__sock, data, len(data))
        else:
            return udt4.sendmsg(self.__sock, data, len(data), padding)
         

    def sendfile(self, fd):
        # method signature wrong
        """
        """
        if isinstance(basestring, fd):
            return udt4.sendfile(self.__sock, fd)
        else:
            return udt4.sendfile(self.__sock, fd.name)


# -----------------------------------------------------------------------------#
# EPOLL                                                                        #

class Epoll(object):
    """
    :class Epoll: 

    Epoll class to act on UdtSocket() class rather than the standard 
    udt4.UDTepoll class which uses udt4.UDTSOCKET 
    """ 

    @property 
    def epoll(self):
        """ 
        :return: internal UDTepoll 
        """ 
        return self.__epoll 


    def __init__(self, _epoll = None):
        """
        :param  _epoll: Initialize _epoll from existing class 
        :type   _epoll: Epoll() 
        """
        if _epoll != None:
            self.__epoll = _epoll.epoll 
        else:
            self.__epoll = udt4.UDTepoll() 
        
        # the socket.socket() library does not allow you to create an object  
        # mapped to an existing fileno(), socket.fromfd(...) actually executes 
        # a dup() on the socket, this may orphan the initial parent and leave
        # fd's unclosed AND the dup'ed socket is not a valid entry in the epoll
        # this keeps a map of socket.socket() --> fileno() entries to prevent
        # the issue. 
        self.__ssock_map = dict() 
        
    
    def garbage_collect(self):
        """
        """ 
        self.__epoll.garbage_collect() 


    def add_usock(self, udt_socket, events = 0x0):
        """
        :param  udt_socket: UDTSOCKET to add to epoll 
        :type   udt_socket: UdtSocket()

        :param  events:     Events to watch on 
        :type   events:     int() 
        """
        return self.__epoll.add_usock(udt_socket.UDTSOCKET, events) 
    

    def add_ssock(self, sys_socket, events = 0x0):
        """
        :param  sys_socket: SYSSOCKET to add to epoll 
        :type   sys_socket: int() 
        
        :param  events:     Ignored  
        :type   events:     int() 
        """
        if not sys_socket.fileno() in self.__ssock_map:
            self.__ssock_map[sys_socket.fileno()] = sys_socket

        return self.__epoll.add_ssock(sys_socket.fileno(), events) 

    
    def remove_usock(self, udt_socket, events = 0x0):
        """
        :param  udt_socket: UDTSOCKET to remove from 
        :type   udt_socket: UdtSocket()          
        
        :param  events:     Events to remove from watch 
        :type   events:     int() 
        """
        return self.__epoll.remove_usock(udt_socket.UDTSOCKET, events) 
    

    def remove_ssock(self, sys_socket, events = 0x0):
        """
        :param  sys_socket: SYSSOCKET to remove to epoll 
        :type   sys_socket: int() 
        
        :param  events:     Ignored  
        :type   events:     int() 
        """
        assert sys_socket.fileno() in self.__ssock_map, \
                'pyudt library error : socket should be in ssmap' 

        del(self.__ssock_map[sys_socket.fileno()])

        return self.__epoll.remove_ssock(sys_socket.fileno()) 

    
    def wait(self, do_uread, do_uwrite, wait, do_sread = False, 
             do_swrite = False):
        """
        :param do_uread:    Wait on a UDTSOCKET read event 
        :type  do_uread:    bool() 

        :param do_uwrite:   Wait on a UDTSOCKET write or error event 
        :type  do_uwrite:   bool() 
    
        :param wait:        Wait time in milliseconds 
        :param wait:        long() 

        :param do_sread:    Wait on a SYSSOCKET read event 
        :type  do_sread:    bool() 

        :param do_swrite:   Wait on a SYSSOCKET write or error event 
        :type  do_swrite:   bool() 


        """
        ret = self.__epoll.wait(do_uread, do_uwrite, wait, do_sread, do_swrite) 
        
        return (
            frozenset(map(lambda s: UdtSocket(_sock = s), ret[0])), 
            frozenset(map(lambda s: UdtSocket(_sock = s), ret[1])), 
            frozenset(map(lambda s: self.__ssock_map[s] , ret[2])),
            frozenset(map(lambda s: self.__ssock_map[s] , ret[3]))
            ) 
