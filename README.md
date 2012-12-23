PyUDT
=====

# About  
This extension is a C++ extension to CPython. And is tested on Linux only.


# Boot-strap 
Note:
The default UDT repo does not specify an install path for the UDT library in its
Makefile.  The attached submodule will by default install it into /usr/lib{,64} and
/usr/include/udt.  If you have installed it in a different location, you may need to
edit the setup.py for the linking step. 

```
#
# ls 
# setup.py README.md lib example ... 
# 
git submodule init 
git submodule update 

# move library and headers to appropriate location 
pushd udt/UDT4 
make
make install 
sudo cp src/libudt.{so,a} /usr/lib64/   # or /usr/lib/ on 32 it
sudo cp src/udt /usr/include/ 
popd 

# build extension 
python setup.py build 
sudo python setup.py install 
```


# The Basics 
`pydoc udt4` and `pydoc udt4.pyudt` are the best sources for documentation.

The library is separated into two primary parts:

* Content imported from udt4 interface with the udt4.UDTSOCKET type which is a direct
representation of the UDTSOCKET C-type and is not callable, but is rather the first 
argument on most udt4 functions. All function signatures attempt to more closely match
the C-api interface.

* Content imported from udt4.pyudt attempts to match the socket.socket() interface 
provided by the standard library.  The class wraps the udt4.UDTSOCKET type. 


## udt4:

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


## pyudt: 

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

