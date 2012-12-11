/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4 is GPLv3, however it must be linked against UDT4
                        libraries to be of use.  UDT4 license is below.
        @date           12/11/2012 

        # - UDT LICENSE ------------------------------------------------------ 
        Copyright (c) 2001 - 2011, The Board of Trustees of the University of 
        Illinois.  All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are
        met:

        * Redistributions of source code must retain the above
          copyright notice, this list of conditions and the
          following disclaimer.

        * Redistributions in binary form must reproduce the
          above copyright notice, this list of conditions
          and the following disclaimer in the documentation
          and/or other materials provided with the distribution.

        * Neither the name of the University of Illinois
          nor the names of its contributors may be used to
          endorse or promote products derived from this
          software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
        IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
        THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
        PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
        CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
        EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
        PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
        PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
        LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
        -----------------------------------------------------------------------
  */

#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "py-udt4.hh"

#ifdef __cplusplus
extern "C" {
#endif 

/* UDT::UDTSOCKET  ---------------------------------------------------------- */
static PyObject*
pyudt4_socket_new(PyTypeObject *type) 
{
        pyudt4_socket_obj *self = (pyudt4_socket_obj*)type->tp_alloc(type, 0);

        self->sock     = 0;
        self->domain   = 0;
        self->type     = 0;
        self->protocol = 0;

        return (PyObject*)self;
}
        
static PyGetSetDef  pyudt4_socket_getset[] = {
        { 0x0 }
};

static PyTypeObject pyudt4_socket_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /* ob_size              */
    "UDTSOCKET",                                /* tp_name              */
    sizeof(pyudt4_socket_obj),                  /* tp_basicsize         */
    0,                                          /* tp_itemsize          */
    0,                                          /* tp_dealloc           */
    0,                                          /* tp_print             */
    0,                                          /* tp_getattr           */
    0,                                          /* tp_setattr           */
    0,                                          /* tp_compare           */
    0,                                          /* tp_repr              */
    0,                                          /* tp_as_number         */
    0,                                          /* tp_as_sequence       */
    0,                                          /* tp_as_mapping        */
    0,                                          /* tp_hash              */
    0,                                          /* tp_call              */
    0,                                          /* tp_str               */
    0,                                          /* tp_getattro          */
    0,                                          /* tp_setattro          */
    0,                                          /* tp_as_buffer         */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags             */
    "UDT4 socket",                              /* tp_doc               */
    0,                                          /* tp_traverse          */
    0,                                          /* tp_clear             */
    0,                                          /* tp_richcompare       */
    0,                                          /* tp_weaklistoffset    */
    0,                                          /* tp_iter              */
    0,                                          /* tp_iternext          */
    0,//pyudt4_socket_methods,                      /* tp_methods           */
    0,                                          /* tp_members           */
    pyudt4_socket_getset,                       /* tp_getset            */
    0,                                          /* tp_base              */
    0,                                          /* tp_dict              */
    0,                                          /* tp_descr_get         */
    0,                                          /* tp_descr_set         */
    0,                                          /* tp_dictoffset        */
    0,//(initproc)pyudt4_socket_init,               /* tp_init              */
    0,                                          /* tp_alloc             */
    0,                                          /* tp_new               */
};

/* UDT::{namespace} --------------------------------------------------------- */ 
static PyObject*
pyudt4_startup(PyObject *py_self) 
{
        Py_RETURN_NONE;
}

static PyObject*
pyudt4_cleanup(PyObject *py_self) 
{
        UDT::cleanup();
        Py_RETURN_NONE;
}

static PyObject*
pyudt4_socket(PyObject *py_self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0; 
        int domain, type, protocol;

        if (!PyArg_ParseTuple(args, "iii", &domain, &type, &protocol)) { 
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [domain = AF_INET] [type] [protocol]"
                        );
                return 0x0;
        }
        
        sock = (pyudt4_socket_obj*)_PyObject_New(&pyudt4_socket_type);

        if (0x0 == sock) {
                PyErr_SetString(
                        PyExc_RuntimeError, "Failed to initialize UDTSOCKET"
                        );
                return 0x0;
        } else {
                sock->domain   = domain;
                sock->type     = type;
                sock->protocol = protocol;
        }
        
        if (AF_INET != domain) {
                PyErr_SetString(
                        PyExc_ValueError, "UDT4 only support AF_INET for domain"
                        );
                return 0x0;
        }

        /* initialize socket */
        sock->sock = UDT::socket(domain, type, protocol);
        
        if (UDT::ERROR == sock->sock) {
                PyErr_SetString(
                        PyExc_RuntimeError, 
                        UDT::getlasterror().getErrorMessage()
                        );
                return 0x0;
        }

        return (PyObject*) sock;
}

static PyObject*
pyudt4_bind(PyObject *py_self, PyObject *args) 
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        sockaddr_in sock_addr;
        char *address = 0x0;
        int   port    = -1 ;
       

        if (!PyArg_ParseTuple(args, "Osi", &sock_obj, &address, &port)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [address] [port]"
                        );

                return Py_BuildValue("i", -2);
        } else {
                /* TODO: assert some sanit in the addres/port */
                sock = (pyudt4_socket_obj*) sock_obj;
        }
        
        int res = 0;
        if ((res = inet_pton(sock->domain, address, &sock_addr.sin_addr))
             == 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid addres (invalid inet_pton(...)"
                        );
                
                return Py_BuildValue("i", res);
        }

        sock_addr.sin_family = sock->domain;
        sock_addr.sin_port   = htons(port);
        
        memset(&sock_addr.sin_zero, '\0', sizeof(sock_addr.sin_zero));

        if (UDT::ERROR == UDT::bind(sock->sock, (sockaddr*) &sock_addr,
                                    sizeof(sock_addr))) {
                PyErr_SetString(
                        PyExc_RuntimeError, 
                        UDT::getlasterror().getErrorMessage()
                        );
                return Py_BuildValue("i", UDT::getlasterror().getErrorCode());
        }

        return Py_BuildValue("i", 0);
}

static PyObject*
pyudt4_listen(PyObject *py_self, PyObject *args) 
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        int backlog = -1;

        if (!PyArg_ParseTuple(args, "Oi", &sock_obj, &backlog)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [backlog]"
                        );

                return Py_BuildValue("i", -1);
        } else if (backlog <= 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid value for backlog (<= 0)"
                        );
                
                return Py_BuildValue("i", -2);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }

        if (UDT::listen(sock->sock, backlog)) {
                PyErr_SetString(
                        PyExc_RuntimeError, 
                        UDT::getlasterror().getErrorMessage()
                        );
        }
                
        return Py_BuildValue("i", 0);

}

static PyObject*
pyudt4_accept(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [backlog]"
                        );
                
                return Py_BuildValue("i", -1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }

        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);
        
        UDTSOCKET client = UDT::accept(sock->sock, (sockaddr*) &client_stg, 
                                       &addr_len);

        if (UDT::INVALID_SOCK == client) {
                PyErr_SetString(
                        PyExc_RuntimeError,
                        UDT::getlasterror().getErrorMessage()
                        );
                
                return Py_BuildValue("i", -1);
        } 

        /* create a socket */
        pyudt4_socket_obj *client_sock = 
                (pyudt4_socket_obj*)_PyObject_New(&pyudt4_socket_type);

        client_sock->domain   = sock->domain;
        client_sock->type     = sock->type;
        client_sock->protocol = sock->protocol;
        
        char client_host[NI_MAXHOST];
        char client_srvc[NI_MAXSERV];

        memset(client_host, '\0', sizeof(client_host));
        memset(client_srvc, '\0', sizeof(client_srvc));

        getnameinfo((sockaddr*) &client_stg, addr_len, 
                    client_host, sizeof(client_host) , 
                    client_srvc, sizeof(client_srvc) ,
                    NI_NUMERICHOST|NI_NUMERICSERV);

        return Py_BuildValue("Us", (PyObject*) client_sock, client_host);
}


static PyMethodDef pyudt4_module_methods[] = {
        {
                "startup",
                (PyCFunction)pyudt4_startup,
                METH_NOARGS,
                "Start the UDT4 library"
        },
        {
                "cleanup",
                (PyCFunction)pyudt4_cleanup,
                METH_NOARGS,
                "Stop the UDT4 library and cleanup resources"
        },
        {
                "socket",
                (PyCFunction)pyudt4_socket,
                METH_VARARGS,
                ""
        },
        {
                "bind",
                (PyCFunction)pyudt4_bind,
                METH_VARARGS,
                ""
        },
        {
                "listen",
                (PyCFunction)pyudt4_listen,
                METH_VARARGS,
                ""
        },
        {
                "accept",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                ""
        },
        { 0x0 }
};

#ifndef PyMODINIT_FUNC 
#define PyMODINIT_FUNC void
#endif 

PyMODINIT_FUNC
initudt4()
{
        PyObject *module;
        
        /* init module */
        if (PyType_Ready(&pyudt4_socket_type) < 0)
                return;
        
        if ((module = Py_InitModule3("udt4", pyudt4_module_methods, 
                        "Python extension for UDT4"
                        )) == 0x0)
                return;
        
        /* add socket objects */
        pyudt4_socket_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_socket_type);
        PyModule_AddObject(module, "UDTSOCKET", (PyObject*)
                           &pyudt4_socket_type);
}

#ifdef __cplusplus
}
#endif 
