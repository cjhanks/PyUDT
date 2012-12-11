#ifndef __PY_UDT_H_
#define __PY_UDT_H_

#include <Python.h>
#include <udt>

typedef struct __pyudt4_socket_object {
        PyObject_HEAD; 
        UDTSOCKET sock;
        int       family;
        int       type;
        int       proto;
} pyudt4_socket_object;


#endif 
