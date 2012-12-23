/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4:        GPLv3 
                        libudt4:        BSD 
        */


#ifndef __PY_UDT_SOCKET_H_
#define __PY_UDT_SOCKET_H_

#include <Python.h>
#include <udt>

#ifdef __cplusplus
extern "C" {
#endif 


typedef struct __pyudt4_socket_obj {
        PyObject_HEAD;          /*< std macro           */ 
        UDTSOCKET sock;         /*< socket implemented  */
        
        int domain;             /*< socket family       */
        int type;               /*< socket type         */
        int protocol;           /*< socket protocol     */
        int valid;              /*< socket is valid     */
} pyudt4_socket_obj;


/**
        Initialize the local socket information to module 
        */
PyTypeObject *initpyudt4_socket_type(PyObject *module);

#ifdef __cplusplus
}
#endif 

#endif // __PY_UDT_SOCKET_H_
