PyUDT
=====

PyUDT is a python binding to the high-performance [UDT library](http://udt.sourceforge.net/).

This extension will only work with CPython on a Linux system.  Some minor fixes may be needed to make it function on
Unix, and there is no attempt to make it functional on Windows.`


## Installation 

In this module is a submodule `./udt` which links to the Sourceforge GIT repo of the UDT development.  Alternatively you
can use your own installation. 

Note that the standard udt Makefile does not specify an install command, the local extension will look in the usual
places, however you may need to change `setup.py` if you have installed to an unusual place.


## Usage 

Using `pydoc udt4` and `pydoct udt4.pyudt` are the best sources for documentation, there exists no external documentation.  
The code is fairly well commented, especially when the interface differs from the C++ API interface. 

There are no documented examples however there are unit tests in `./test/` which accomplish more or less the same goal.

The library is broken up into two independent parts:
*	Content imported from the `udt4` interface 
*	Content imported from the `udt4.pyudt` interface 

### udt4 
```python
import udt4 as udt 
```

The plain `udt4` import seeks to have a near 1-to-1 correspondence with the base UDT C++ library.  There are no classes,
only function calls where the first argument is a reference to a `udt4.UDTSOCKET` type, which is internally represented
by the UDTSOCKET C-type (though for convenience reasons retains some information about how it was created).

__udt4.UDTSOCKET__ 
```python
# Raw socket object. 

Classes designed to be operated on by udt4 functions.

socket = udt4.socket( ... ) 
print(socket)                   # <type 'UDTSOCKET'> 
```

__udt4.UDTepoll__ 
```python
epoll  = udt4.UDTepoll() 
print(epoll)                    # <type 'UDTepoll'>  

epoll.add_usock(<type 'UDTSOCKET'> socket, int(flags)) 
```



### udt4.pyudt 
```python
from udt4 import pyudt as udt  
```

The `pyudt` implementation tries to more closely match the python socket interface where possible.  Not all functions
available in socket are available in `pyudt.UdtSocket()`.


Classes designed to work with other pyudt classes.

__pyudt.UdtSocket()__ 
```python
socket = pyudt.UdtSocket( ... ) 
print(socket)                   # <udt4.pyudt.UdtSocket object>  
```  

__pyudt.Epoll()__ 
```python 
epoll = pyudt.Epoll() 
print(epoll)                    # <udt4.pyudt.Epoll object> 

epoll.add_usock(<udt4.pyudt.UdtSocket object> socket, int(flags)) 
```

