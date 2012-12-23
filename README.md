PyUDT
=====

## About  
This extension is a C++ extension to CPython. And is tested on Linux only.


## Boot-strap 
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


## The Basics 
`pydoc udt4` and `pydoc udt4.pyudt` are the best sources for documentation.

The library is separated into two primary parts:

* Content imported from udt4 interface with the udt4.UDTSOCKET type which is a direct
representation of the UDTSOCKET C-type and is not callable, but is rather the first 
argument on most udt4 functions. All function signatures attempt to more closely match
the C-api interface.

* Content imported from udt4.pyudt attempts to match the socket.socket() interface 
provided by the standard library.  The class wraps the udt4.UDTSOCKET type. 


## Classes:

___udt4.TRACEINFO__
```
Please see UDT::TRACEINFO in C-api or pydoc udt4.TRACEINFO for all members.  
```

__udt4.UDTSOCKET__ 
```
.
```


### Sockets:

__udt4.UDTSOCKET__ 



__udt4.pyudt__





