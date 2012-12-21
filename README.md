PyUDT
=====

## About  
The py-udt4 library is segmented into two primary imports.

### udt4 
```python 
import udt4
```

This imports the Python UDT library which most closely mocks the C++ api.  All 
functions returned by reference / pointer instead use the Python convention of
returning by tuple().  `pydoc udt4` documents the paramater and return types.


### pyudt 
```python 
import udt4 as udt 
import udt4.pyudt as pyudt
``` 

The udt4 import is required for access to the enumerators and instance constants
associated with the UDT library.

The pyudt implementation create UdtSocket() instances which more closely resemble
the python socket.socket() implementation.  It also includes Epoll classes which 
are more 'pythonic' in interface.  See `pydoc udt4.pyudt` for documentation.


## Boot-strap 
Note:
The default UDT repo does not specify an install path for the UDT library in its
Makefile.  The attached submodule will by default install it into /usr/lib{,64} and
/usr/include/udt.  If you have installed it in a different location, you may need to
edit the setup.py for the linking step. 

```
#
# pwd 
# .../PyUDT/ 
# ls 
# .. .. setup.py .. .. 
# 
git submodule init 
git submodule update 

pushd udt/UDT4 
make
make install 
popd 

python setup.py build 
sudo python setup.py install 
```


