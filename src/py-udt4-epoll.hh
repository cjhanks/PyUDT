/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4:        GPLv3 
                        libudt4:        BSD 
        */

#ifndef __PY_UDT_EPOLL_H_
#define __PY_UDT_EPOLL_H_

#include <Python.h>
#include <vector> 

#include "py-udt4-socket.hh"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct __pyudt4_epoll_obj{
        PyObject_HEAD;
        
        /**  epoll id */
        int eid;        

        /** UDTSOCKET eid sets */
        std::vector<UDTSOCKET> socks; 
        //std::map<UDTSOCKET, pyudt4_socket_obj*> readfds, writefds, errfds;
        
} pyudt4_epoll_obj;


/**
        Initialize the epoll type to module 
        */
PyTypeObject* 
initpyudt4_epoll_type(PyObject *module, PyObject *exception_type, 
                      PyTypeObject *socket_type);

#ifdef __cplusplus
}
#endif 
#endif //__PY_UDT_EPOLL_H_
