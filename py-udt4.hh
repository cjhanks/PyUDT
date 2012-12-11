#ifndef __PY_UDT_H_
#define __PY_UDT_H_

#include <Python.h>
#include <udt>

typedef struct __pyudt4_socket_obj {
        PyObject_HEAD;          /*< std macro           */ 
        UDTSOCKET sock;         /*< socket implemented  */
        
        /* for debug and assertions */
        int       domain;       /*< socket family       */
        int       type;         /*< socket type         */
        int       protocol;     /*< socket protocol     */
} pyudt4_socket_obj;


#endif 
