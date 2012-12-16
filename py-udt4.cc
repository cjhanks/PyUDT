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
#include <assert.h> 

#include <iostream>
#include <fstream> 
#include <set>
#include <map> 

#include "py-udt4.hh"

#define DEBUG(_msg_) fprintf(stderr, _msg_) 

#define RETURN_UDT_RUNTIME_ERROR \
        do {\
                PyErr_SetObject(\
                        PyExc_RuntimeError, \
                       _PyObject_New(&pyudt4_exception_type)\
                        );\
                \
                return 0x0;\
        } while (0)



#ifdef __cplusplus
extern "C" {
#endif 

/* -------------------------------------------------------------------------- */
/* UDT::EXCEPTION  ---------------------------------------------------------- */
typedef struct __pyudt4_exception_obj {
        PyObject_HEAD;
        int       code; 
        PyObject *message;
} pyudt4_exception_obj;



static PyObject*
pyudt4_exception_tp_str(PyObject *py_self)
{
        return ((pyudt4_exception_obj*) py_self)->message;
}


static PyObject*
pyudt4_exception_init(PyTypeObject *type) 
{
        pyudt4_exception_obj *self =
                (pyudt4_exception_obj*) type->tp_alloc(type, 0);

        self->code    = UDT::getlasterror().getErrorCode();
        self->message = Py_BuildValue("s", 
                                UDT::getlasterror().getErrorMessage()
                                );
        
        UDT::getlasterror().clear();

        return (PyObject*) self;
}

/*
static PyObject*
pyudt4_exception_tp_as_number(PyObject *py_self)
{
        return Py_BuildVale("i", 
                        ((pyudt4_exception_obj*) py_self)->code
                        );;
}
*/

/**
        TODO:
        error code accessor method need (either by getattr OR tp_as_number 
        */
static PyTypeObject pyudt4_exception_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /* ob_size              */
    "UDTException",                             /* tp_name              */
    sizeof(pyudt4_exception_obj),               /* tp_basicsize         */
    0,                                          /* tp_itemsize          */
    0,                                          /* tp_dealloc           */
    0,                                          /* tp_print             */
    0,                                          /* tp_getattr           */
    0,                                          /* tp_setattr           */
    0,                                          /* tp_compare           */
    0,                                          /* tp_repr              */
    0, //pyudt4_exception_tp_as_number,              /* tp_as_number         */
    0,                                          /* tp_as_sequence       */
    0,                                          /* tp_as_mapping        */
    0,                                          /* tp_hash              */
    0,                                          /* tp_call              */
    pyudt4_exception_tp_str,                    /* tp_str               */
    0,                                          /* tp_getattro          */
    0,                                          /* tp_setattro          */
    0,                                          /* tp_as_buffer         */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags             */
    "UDT4 Exception",                           /* tp_doc               */
    0,                                          /* tp_traverse          */
    0,                                          /* tp_clear             */
    0,                                          /* tp_richcompare       */
    0,                                          /* tp_weaklistoffset    */
    0,                                          /* tp_iter              */
    0,                                          /* tp_iternext          */
    0,                                          /* tp_methods           */
    0,                                          /* tp_members           */
    0,                                          /* tp_getset            */
    0,                                          /* tp_base              */
    0,                                          /* tp_dict              */
    0,                                          /* tp_descr_get         */
    0,                                          /* tp_descr_set         */
    0,                                          /* tp_dictoffset        */
    (initproc)pyudt4_exception_init,            /* tp_init              */
    0,                                          /* tp_alloc             */
    0,                                          /* tp_new               */
};


/* -------------------------------------------------------------------------- */
/* UDT::UDTSOCKET  ---------------------------------------------------------- */
static PyObject*
pyudt4_socket_init(PyTypeObject *type) 
{
        pyudt4_socket_obj *self = (pyudt4_socket_obj*)type->tp_alloc(type, 0);

        self->sock     = 0;
        self->domain   = 0;
        self->type     = 0;
        self->protocol = 0;
        
        return (PyObject*)self;
}

static PyObject*
pyudt4_socket_debug(pyudt4_socket_obj *self)
{
        fprintf(stderr,
                "------------\n"
                "Socket:   %i\n"
                "Domain:   %i\n"
                "Type:     %i\n"
                "Protocol: %i\n"
                "------------\n", self->sock, self->domain, self->type,
                                  self->protocol
               );

        Py_RETURN_NONE;
}

static PyMethodDef pyudt4_socket_methods[] = {
        {
                "debug",
                (PyCFunction)pyudt4_socket_debug,
                METH_NOARGS,
                ""
        },
        { 0x0 }
};


static PyObject*
pyudt4_socket_get_domain(pyudt4_socket_obj *self)
{
        return Py_BuildValue("i", self->domain);
}


static PyObject*
pyudt4_socket_get_type(pyudt4_socket_obj *self)
{
        return Py_BuildValue("i", self->type);
}


static PyObject*
pyudt4_socket_get_protocol(pyudt4_socket_obj *self)
{
        return Py_BuildValue("i", self->protocol);
}


static PyObject*
pyudt4_socket_get_udtsocket(pyudt4_socket_obj *self)
{
        return Py_BuildValue("i", self->sock);
}
        
        
static PyGetSetDef  pyudt4_socket_getset[] = {
        {
                (char*) "family",
                (getter) pyudt4_socket_get_domain,
                (setter) 0x0,
                (char*) "address domain"
        }, 
        {       /* intential duplicate */
                (char*) "domain",
                (getter) pyudt4_socket_get_domain,
                (setter) 0x0,
                (char*) "address domain"
        },
        {
                (char*) "type",
                (getter) pyudt4_socket_get_type,
                (setter) 0x0,
                (char*) "address type"
        },
        {
                (char*) "protocol",
                (getter) pyudt4_socket_get_protocol,
                (setter) 0x0,
                (char*) "address protocol"
        }, 
        {
                (char*) "udtsocket",
                (getter) pyudt4_socket_get_udtsocket,
                (setter) 0x0,
                (char*) "underlying udtsocket"
        }, 
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
    pyudt4_socket_methods,                      /* tp_methods           */
    0,                                          /* tp_members           */
    pyudt4_socket_getset,                       /* tp_getset            */
    0,                                          /* tp_base              */
    0,                                          /* tp_dict              */
    0,                                          /* tp_descr_get         */
    0,                                          /* tp_descr_set         */
    0,                                          /* tp_dictoffset        */
    (initproc)pyudt4_socket_init,               /* tp_init              */
    0,                                          /* tp_alloc             */
    0,//pyudt4_socket_new,                          /* tp_new               */
};


/* -------------------------------------------------------------------------- */
/* UDT::EPOLL --------------------------------------------------------------- */
typedef struct __pyudt4_epoll_obj{
        PyObject_HEAD;
        
        /**  epoll id */
        int eid;        

        /** UDTSOCKET eid sets */
        std::map<UDTSOCKET, pyudt4_socket_obj*> readfds, writefds, errfds;
        
} pyudt4_epoll_obj;


static int
pyudt4_epoll_init(pyudt4_epoll_obj *self)
{
        self->eid = UDT::epoll_create();
        
        /* C - api does not initialize C++ ctors */
        self->readfds  = std::map<UDTSOCKET, pyudt4_socket_obj*>();
        self->writefds = std::map<UDTSOCKET, pyudt4_socket_obj*>();
        self->errfds   = std::map<UDTSOCKET, pyudt4_socket_obj*>();
        
        return 0;
}


static void
pyudt4_epoll_dealloc(pyudt4_epoll_obj *self)
{
        /* decrement the references */
        for (std::map<UDTSOCKET, pyudt4_socket_obj*>::iterator i = 
             self->readfds.begin(); i != self->readfds.end(); ++i)
                Py_XDECREF(i->second);
        
        for (std::map<UDTSOCKET, pyudt4_socket_obj*>::iterator i = 
             self->writefds.begin(); i != self->writefds.end(); ++i)
                Py_XDECREF(i->second);
        
        for (std::map<UDTSOCKET, pyudt4_socket_obj*>::iterator i = 
             self->errfds.begin(); i != self->errfds.end(); ++i)
                Py_XDECREF(i->second);
        

        /* remove id from epoll */
        UDT::epoll_release(self->eid);
}


static PyObject*
pyudt4_epoll_add_usock(pyudt4_epoll_obj *self, PyObject *args)
{
        pyudt4_socket_obj *sock;
        
        int flag = 0x0;

        if (!PyArg_ParseTuple(args, "O|i", &sock, &flag)) {
                PyErr_SetString(PyExc_TypeError,
                        "Invalid usock"
                        );

                return 0x0;
        }
        
        /* increase the reft for each mapper */
        if (0x0 == flag || flag & 0x2) {
                Py_XINCREF(sock);
                self->readfds.insert(std::make_pair(sock->sock, sock));
        }
        
        if (0x0 == flag || flag & 0x4) {
                Py_XINCREF(sock);
                self->writefds.insert(std::make_pair(sock->sock, sock));
        }
        
        if (0x0 == flag || flag & 0x8) {
                Py_XINCREF(sock);
                self->errfds.insert(std::make_pair(sock->sock, sock));
        }

        if (UDT::ERROR == UDT::epoll_add_usock(self->eid, sock->sock, &flag))
                RETURN_UDT_RUNTIME_ERROR;

        Py_RETURN_NONE;        
}


static PyObject*
pyudt4_epoll_add_ssock(pyudt4_epoll_obj *self, PyObject *args) 
{
        SYSSOCKET sock;
        int       flag = 0x0;

        if (!PyArg_ParseTuple(args, "i|i", &sock, &flag)) {
                PyErr_SetString(PyExc_TypeError,
                        "Invalid usock"
                        );

                return 0x0;
        }
        
        if (UDT::ERROR == UDT::epoll_add_ssock(self->eid, sock, &flag))
                RETURN_UDT_RUNTIME_ERROR;

        Py_RETURN_NONE;        
}


static PyObject*
pyudt4_epoll_remove_usock(pyudt4_epoll_obj *self, PyObject *args)
{
        pyudt4_socket_obj *sock;
        
        int flag = 0x0;

        if (!PyArg_ParseTuple(args, "O|i", &sock, &flag)) {
                PyErr_SetString(PyExc_TypeError,
                        "Invalid usock"
                        );

                return 0x0;
        }
        
        /* increase the reft for each mapper */
        if (0x0 == flag || flag & 0x2) {
                Py_XDECREF(sock);
                self->readfds.insert(std::make_pair(sock->sock, sock));
        }
        
        if (0x0 == flag || flag & 0x4) {
                Py_XDECREF(sock);
                self->writefds.insert(std::make_pair(sock->sock, sock));
        }
        
        if (0x0 == flag || flag & 0x8) {
                Py_XDECREF(sock);
                self->errfds.insert(std::make_pair(sock->sock, sock));
        }

        if (UDT::ERROR == UDT::epoll_add_usock(self->eid, sock->sock, &flag))
                RETURN_UDT_RUNTIME_ERROR;

        Py_RETURN_NONE;
}


static PyObject*
pyudt4_epoll_remove_ssock(pyudt4_epoll_obj *self, PyObject *args)
{
        int sock;
        int flag = 0x0;

        if (!PyArg_ParseTuple(args, "i|i", &sock, &flag)) {
                PyErr_SetString(PyExc_TypeError,
                        "Invalid usock"
                        );

                return 0x0;
        }

        if (UDT::ERROR == UDT::epoll_add_ssock(self->eid, sock, &flag))
                RETURN_UDT_RUNTIME_ERROR;
        
        Py_RETURN_NONE;
}


static PyObject*
pyudt4_epoll_wait(pyudt4_epoll_obj *self, PyObject *args)
{
        long timeo;

        PyObject *do_uread, *do_uwrite;
        PyObject *do_sread = Py_False, *do_swrite = Py_False;
        
        if (!PyArg_ParseTuple(args, "OOi|OO", &do_uread, &do_uwrite, &timeo, 
                              &do_sread, &do_swrite)) {
                PyErr_SetString(PyExc_TypeError,
                        "invalid epoll args"
                        );

                return 0x0;
        }

        struct {
                std::set<UDTSOCKET> read;
                std::set<UDTSOCKET> write;
        } usock, ssock;

        int rc = UDT::epoll_wait(self->eid,
                        do_uread  == Py_True ? &usock.read  : 0x0,
                        do_uwrite == Py_True ? &usock.write : 0x0,
                        timeo,
                        do_sread  == Py_True ? &ssock.read  : 0x0,
                        do_swrite == Py_True ? &ssock.write : 0x0
                        );

        if (UDT::ERROR == rc)
                RETURN_UDT_RUNTIME_ERROR;

        /* load the sets */
        struct {
                PyObject *read;
                PyObject *write;
        } 
        uset = {
                PyFrozenSet_New(0x0), 
                PyFrozenSet_New(0x0)
        }, 
        sset = {
                PyFrozenSet_New(0x0), 
                PyFrozenSet_New(0x0)
        };
        
        /* UDTSOCKET sets */
        for (std::set<UDTSOCKET>::iterator i = usock.read.begin();
             i != usock.read.end(); ++i) {
                pyudt4_socket_obj *sock = self->readfds[*i];
                Py_XINCREF(sock);
                PySet_Add(uset.read , (PyObject*) sock); 
        }
        
        for (std::set<UDTSOCKET>::iterator i = usock.read.begin();
             i != usock.read.end(); ++i) {
                pyudt4_socket_obj *sock = self->writefds[*i];
                Py_XINCREF(sock);
                PySet_Add(uset.write, (PyObject*) sock); 
        }

        /* SYSSOCKET sets */
        for (std::set<UDTSOCKET>::iterator i = ssock.read.begin();
             i != usock.read.end(); ++i) {
                PySet_Add(sset.read ,  Py_BuildValue("i", *i));
        }
        
        for (std::set<UDTSOCKET>::iterator i = ssock.read.begin();
             i != usock.read.end(); ++i) {
                PySet_Add(sset.write,  Py_BuildValue("i", *i));
        }
        
        return Py_BuildValue("OOOO", uset.read, uset.write, 
                             sset.read, sset.write);
}


static PyMethodDef pyudt4_epoll_methods[] = {
        {
                "add_usock",
                (PyCFunction)pyudt4_epoll_add_usock,
                METH_VARARGS,
                "add a UDT socket to poll"
        },
        {
                "add_ssock",
                (PyCFunction)pyudt4_epoll_add_ssock,
                METH_VARARGS,
                "add a System socket to poll [eg: TCP socket fd]"
        },
        {
                "add_remove_usock",
                (PyCFunction)pyudt4_epoll_add_usock,
                METH_VARARGS,
                "add a UDT socket to poll"
        },
        {
                "add_remove_ssock",
                (PyCFunction)pyudt4_epoll_add_ssock,
                METH_VARARGS,
                "add a UDT socket to poll"
        },
        {
                "wait",
                (PyCFunction)pyudt4_epoll_wait,
                METH_VARARGS,
                "add a UDT socket to poll"
        },
        { 0x0 }
};

static PyTypeObject pyudt4_epoll_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /* ob_size              */
    "UDTepoll",                                 /* tp_name              */
    sizeof(pyudt4_epoll_obj),                   /* tp_basicsize         */
    0,                                          /* tp_itemsize          */
    (destructor)pyudt4_epoll_dealloc,           /* tp_dealloc           */
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
    "UDT4 Epoll",                               /* tp_doc               */
    0,                                          /* tp_traverse          */
    0,                                          /* tp_clear             */
    0,                                          /* tp_richcompare       */
    0,                                          /* tp_weaklistoffset    */
    0,                                          /* tp_iter              */
    0,                                          /* tp_iternext          */
    pyudt4_epoll_methods,                       /* tp_methods           */
    0,                                          /* tp_members           */
    0,                                          /* tp_getset            */
    0,                                          /* tp_base              */
    0,                                          /* tp_dict              */
    0,                                          /* tp_descr_get         */
    0,                                          /* tp_descr_set         */
    0,                                          /* tp_dictoffset        */
   (initproc)pyudt4_epoll_init,                 /* tp_init              */
    0,                                          /* tp_alloc             */
    0,                                          /* tp_new               */
};


/* -------------------------------------------------------------------------- */
/* UDT::{namespace} --------------------------------------------------------- */ 
static PyObject*
pyudt4_startup(PyObject *py_self) 
{
        UDT::startup();
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
                RETURN_UDT_RUNTIME_ERROR;
        }

        return (PyObject*) sock;
}


static PyObject*
pyudt4_bind(PyObject *py_self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0;
        
        sockaddr_in sock_addr;
        char *address = 0x0;
        int   port    = -1 ;
       
        if (!PyArg_ParseTuple(args, "Osi", &sock, &address, &port)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [address] [port]"
                        );
                
                return 0x0;
        }
        
        if (inet_pton(sock->domain, address, &sock_addr.sin_addr) == 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid address (invalid inet_pton(...)"
                        );
                
                return 0x0;
        }

        sock_addr.sin_family      = sock->domain;
        sock_addr.sin_port        = htons(port);
        //sock_addr.sin_addr.s_addr = INADDR_ANY; 

        memset(&sock_addr.sin_zero, '\0', sizeof(sock_addr.sin_zero));

        if (UDT::ERROR == UDT::bind(sock->sock, (sockaddr*) &sock_addr,
                                    sizeof(sock_addr))) {
                RETURN_UDT_RUNTIME_ERROR;
        }

        return Py_BuildValue("i", 0);
}

static PyObject*
pyudt4_listen(PyObject *py_self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0;

        int backlog = -1;

        if (!PyArg_ParseTuple(args, "Oi", &sock, &backlog)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [backlog]"
                        );
        
                return 0x0;
        } else if (backlog <= 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid value for backlog (<= 0)"
                        );
                
                return 0x0;
        }
        
        if (UDT::listen(sock->sock, backlog)) {
                RETURN_UDT_RUNTIME_ERROR;
        }
                
        return Py_BuildValue("i", 0);

}

static PyObject*
pyudt4_accept(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [backlog]"
                        );
                
                return 0x0;
        }

        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);
        
        UDTSOCKET client = UDT::accept(sock->sock, (sockaddr*) &client_stg, 
                                       &addr_len);

        if (UDT::INVALID_SOCK == client) {
                RETURN_UDT_RUNTIME_ERROR;
        } 

        /* create a socket */
        pyudt4_socket_obj *client_sock = 
                (pyudt4_socket_obj*)_PyObject_New(&pyudt4_socket_type);
        
        client_sock->sock     = client;
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
        
        return Py_BuildValue("Os", (PyObject*) client_sock, client_host); 
}


static PyObject*
pyudt4_connect(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        char *host = 0x0;
        int   port = -1;;
        
        if (!PyArg_ParseTuple(args, "Osi", &sock, &host, &port)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );

                return 0x0; 
        } 

        sockaddr_in serv;
        serv.sin_family = sock->domain;
        serv.sin_port   = htons(port);

        if (inet_pton(sock->domain, host, &serv.sin_addr) <= 0) {
                PyErr_SetString(
                        PyExc_RuntimeError,
                        "Failed to set the family"
                        );
                
                return 0x0;
        } else {
                memset(&serv.sin_zero, '\0', sizeof(serv.sin_zero));
        }        
        
        if (UDT::ERROR == UDT::connect(sock->sock, (sockaddr*) &serv, 
                                       sizeof(serv))) {
                RETURN_UDT_RUNTIME_ERROR;
        }
        
        return Py_BuildValue("i", 0);
}


static PyObject*
pyudt4_close(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        } 

        return Py_BuildValue("i", UDT::close(sock->sock));
} 


static PyObject*
pyudt4_getpeername(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        }
        
        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);

        if (UDT::ERROR == UDT::getpeername(sock->sock, (sockaddr*) &client_stg,
                                           &addr_len)) {
                RETURN_UDT_RUNTIME_ERROR;
        }

        char client_host[NI_MAXHOST];
        char client_srvc[NI_MAXSERV];

        memset(client_host, '\0', sizeof(client_host));
        memset(client_srvc, '\0', sizeof(client_srvc));

        getnameinfo((sockaddr*) &client_stg, addr_len, 
                    client_host, sizeof(client_host) , 
                    client_srvc, sizeof(client_srvc) ,
                    NI_NUMERICHOST|NI_NUMERICSERV);
       
        return Py_BuildValue(
                        "si", client_host, ntohs(client_stg.sin_port)
                        ); 
} 


static PyObject*
pyudt4_getsockname(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        }
        
        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);

        if (UDT::ERROR == UDT::getsockname(sock->sock, (sockaddr*) &client_stg,
                                           &addr_len)) {
                RETURN_UDT_RUNTIME_ERROR;
        }

        char client_host[NI_MAXHOST];
        char client_srvc[NI_MAXSERV];

        memset(client_host, '\0', sizeof(client_host));
        memset(client_srvc, '\0', sizeof(client_srvc));

        getnameinfo((sockaddr*) &client_stg, addr_len, 
                    client_host, sizeof(client_host) , 
                    client_srvc, sizeof(client_srvc) ,
                    NI_NUMERICHOST|NI_NUMERICSERV);
       
        return Py_BuildValue(
                        "si", client_host, ntohs(client_stg.sin_port)
                        );
} 


static PyObject*
pyudt4_setsockopt(PyObject *py_self, PyObject *args)
{
        int rc; 

        pyudt4_socket_obj *sock = 0x0;
        
        int          optname_i;
        UDT::SOCKOPT optname;
        long         optval_o;
         
        if (!PyArg_ParseTuple(args, "Oil", &sock, &optname_i, &optval_o)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [OPTION] [OPTVALUE]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                optname = (UDT::SOCKOPT) optname_i;
        }
        
        //DEBUG("here\n");
        
        switch (optname) {
        
        /* int cases */
        case UDT_MSS     :
        case UDT_FC      :
        case UDT_SNDBUF  :
        case UDT_RCVBUF  :
        case UDP_SNDBUF  :
        case UDP_RCVBUF  :
        case UDT_SNDTIMEO:
        case UDT_RCVTIMEO:
        case UDT_MAXBW   : {

                if (UDT_MAXBW == optname) {
                        long optval = optval_o;
                        if (
                            UDT::ERROR == 
                            (rc = UDT::setsockopt(sock->sock, 0, optname, 
                                                  &optval, sizeof(long)))
                            ) 
                                RETURN_UDT_RUNTIME_ERROR;
                } else {
                        int optval = optval_o; 

                        if (
                            UDT::ERROR == 
                            (rc = UDT::setsockopt(sock->sock, 0, optname, 
                                                  &optval, sizeof(int)))
                            ) {
                                RETURN_UDT_RUNTIME_ERROR;
                        }
                }
        
                return Py_BuildValue("i", rc);
        }

        case UDT_SNDSYN    :
        case UDT_RCVSYN    :
        case UDT_RENDEZVOUS:
        case UDT_REUSEADDR : {
                PyObject *py_optval;        
                bool optval;
               
                if (!PyArg_ParseTuple(args, "OiO", &sock, &optname_i, 
                                      &py_optval)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "invalid arguments for [OiO]" 
                                );
                        return Py_BuildValue("i", -1);
                }

                if      (Py_True  == py_optval) {
                        optval = true;
                } else if (Py_False == py_optval) {
                        optval = false;
                } else {
                        PyErr_SetString(
                                PyExc_ValueError,
                                "3rd option must be of bool type"
                                );
                        return Py_BuildValue("i", -1);
                }

                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname,
                                                        &optval, sizeof(bool))))
                        RETURN_UDT_RUNTIME_ERROR;
                
                return Py_BuildValue("i", rc); 
        }

        case UDT_LINGER: {
                linger optval;

                if (!PyArg_ParseTuple(args, "Oiii", &sock, 0, 
                                      &optval.l_onoff, &optval.l_linger)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "invalid arguments for linger" 
                                );
                        
                        return 0x0;
                }

                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname,
                                                     &optval, sizeof(optval))))
                        RETURN_UDT_RUNTIME_ERROR;

                return Py_BuildValue("i", rc);
        }

        default: {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid option name"  
                        );
                
                return 0x0;
        }
        }
        
        return Py_BuildValue("i", -1);

} 


static PyObject*
pyudt4_getsockopt(PyObject *py_self, PyObject *args)
{
        int rc; 

        pyudt4_socket_obj *sock = 0x0;
        
        int          optname_i;
        UDT::SOCKOPT optname;
        int          len;

        if (!PyArg_ParseTuple(args, "Oi", &sock, &optname_i)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        } else {
                optname = (UDT::SOCKOPT) optname_i;
        }
        
        switch (optname) {
        case UDT_MSS     :
        case UDT_FC      :
        case UDT_SNDBUF  :
        case UDT_RCVBUF  :
        case UDP_SNDBUF  :
        case UDP_RCVBUF  :
        case UDT_SNDTIMEO:
        case UDT_RCVTIMEO: {
                int optval;
                len = sizeof(optval);
                if (
                    UDT::ERROR == 
                    (rc = UDT::getsockopt(sock->sock, 0, optname, 
                                          &optval, &len))
                    ) {
                        RETURN_UDT_RUNTIME_ERROR;
                } else {
                        return Py_BuildValue("l", optval);
                }
        }

        case UDT_MAXBW   : {
                long optval; 
                if (
                    UDT::ERROR == 
                    (rc = UDT::getsockopt(sock->sock, 0, optname, 
                                          &optval, &len))
                    ) {
                        RETURN_UDT_RUNTIME_ERROR;
                }
                        
                return Py_BuildValue("i", optval);
        }

        case UDT_SNDSYN    :
        case UDT_RCVSYN    :
        case UDT_RENDEZVOUS:
        case UDT_REUSEADDR : {
                bool optval;
                
                if (UDT::ERROR == (rc = UDT::getsockopt(sock->sock, 0, optname,
                                                        &optval, &len))) {
                        RETURN_UDT_RUNTIME_ERROR;
                } else {
                        return (optval) ? Py_True : Py_False;
                }
        }

        case UDT_LINGER: {
                linger optval;

                if (UDT::ERROR == (rc = UDT::getsockopt(sock->sock, 0, optname,
                                                        &optval, &len))) {
                        RETURN_UDT_RUNTIME_ERROR;
                } else {
                        return Py_BuildValue(
                                        "ii", optval.l_onoff, optval.l_linger
                                        );
                }
        }

        default: {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid option name"  
                        );
                
                return 0x0;
        }
        }
        
        return Py_BuildValue("i", -1);
}

static PyObject*
pyudt4_send(PyObject *py_self, PyObject *args)
{
        int rc;

        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;  /* true buffer length    */
        int   pref_len; /* passed in length size */

        if (!PyArg_ParseTuple(args, "Os#i", &sock, &buf, &buf_len, 
                              &pref_len)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [BUFFER] [BUFFER LENGTH]"
                        );
                
                return 0x0;
        } else {
                if ((pref_len - buf_len) > buf_len) {
                        PyErr_SetString(
                                PyExc_ValueError,
                                "preferred length must not double real "
                                "buffer length"
                                );

                        return 0x0;
                }
        }
        
        if (pref_len > buf_len) {
                rc = UDT::send(sock->sock, buf, buf_len, 0);
                
                if (UDT::ERROR == rc) 
                        RETURN_UDT_RUNTIME_ERROR;

                /* send remainder */
                memset(buf, '\0', pref_len - buf_len);
                
                rc = UDT::send(sock->sock, buf, pref_len - buf_len, 0);
        } else {
                rc = UDT::send(sock->sock, buf, buf_len, 0);
        }
                
        if (UDT::ERROR == rc)   
                RETURN_UDT_RUNTIME_ERROR;

        return Py_BuildValue("i", rc);
} 


static PyObject*
pyudt4_sendmsg(PyObject *py_self, PyObject *args)
{
        int rc;

        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;  /* true buffer length    */
        int   pref_len; /* passed in length size */
        int   ttl = -1;
        PyObject *in_order = Py_False;

        if (!PyArg_ParseTuple(args, "Os#i|iO", &sock, &buf, &buf_len, 
                              &pref_len, &ttl, &in_order)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [BUFFER] [BUFFER LENGTH]"
                        );
                
                return 0x0;
        } else {
                if ((pref_len - buf_len) > buf_len) {
                        PyErr_SetString(
                                PyExc_ValueError,
                                "preferred length must not double real "
                                "buffer length"
                                );

                        return 0x0;
                }
        }
        
        if (pref_len > buf_len) {
                /*
                   udp dgram packets must be sent in a single send or those
                   with in_order = false
                   */
                char *new_buf = (char*)PyMem_Malloc(sizeof(char) * pref_len);
                
                memcpy(new_buf, buf , buf_len );
                memset(&new_buf[buf_len], '\0', pref_len - buf_len);
                        
                rc = UDT::sendmsg(sock->sock, new_buf, pref_len, ttl, 
                                  in_order != Py_False);

                PyMem_Free(new_buf);
        } else {
                rc = UDT::sendmsg(sock->sock, buf, buf_len, ttl, 
                                  in_order != Py_False);
        }
        
        if (UDT::ERROR == rc) 
                RETURN_UDT_RUNTIME_ERROR;

        return Py_BuildValue("i", rc);
} 


static PyObject*
pyudt4_recv(PyObject *py_self, PyObject *args)
{
        int rc; 

        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;

        if (!PyArg_ParseTuple(args, "Oi", &sock, &buf_len)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [INT]" 
                        );
                
                return 0x0;
        }

        buf = (char*)PyMem_Malloc(sizeof(char) * buf_len);

        if (0x0 == buf) {
                PyErr_SetString(
                        PyExc_RuntimeError, 
                        "memory allocation error"
                        );

                return 0x0;
        }

        memset(buf, '\0', buf_len);

        rc = UDT::recv(sock->sock, buf, buf_len, 0);

        if (UDT::ERROR == rc) {
                PyMem_Free(buf);
                RETURN_UDT_RUNTIME_ERROR;
        }

        PyObject *ret = Py_BuildValue("s#", buf, buf_len);
        PyMem_Free(buf);

        return ret;
} 


static PyObject*
pyudt4_recvmsg(PyObject *py_self, PyObject *args)
{
        int rc; 

        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;

        if (!PyArg_ParseTuple(args, "Oi", &sock, &buf_len)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [INT]" 
                        );
                
                return 0x0;
        }

        buf = (char*)PyMem_Malloc(sizeof(char) * buf_len);

        if (0x0 == buf) {
                PyErr_SetString(
                        PyExc_RuntimeError, 
                        "memory allocation error"
                        );

                return 0x0;
        }

        memset(buf, '\0', buf_len);

        rc = UDT::recvmsg(sock->sock, buf, buf_len);

        if (UDT::ERROR == rc) {
                PyMem_Free(buf);
                RETURN_UDT_RUNTIME_ERROR;
        }

        PyObject *ret = Py_BuildValue("s#", buf, buf_len);
        PyMem_Free(buf);

        return ret;
} 

static PyObject*
pyudt4_sendfile(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        char *fname;
        long  offset;
        long  size;
        int   block = 7320000;

        if (!PyArg_ParseTuple(args, "Osll|l", &sock, &fname, &offset, &size,
                              &block)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "ERROR HERE"
                        );

                return 0x0;
        }
        
        
        std::fstream fstrm(fname);
        
        if (UDT::ERROR == UDT::sendfile(sock->sock, fstrm, offset, size, block))
                RETURN_UDT_RUNTIME_ERROR;
        
        return Py_BuildValue("i", 0);
} 


static PyObject*
pyudt4_recvfile(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        char *fname;
        long  offset;
        long  size;
        int   block = 366000;

        if (!PyArg_ParseTuple(args, "Osll|l", &sock, &fname, &offset, &size,
                              &block)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "ERROR HERE"
                        );

                return 0x0;
        }
        
        std::fstream fstrm(fname);

        if (UDT::ERROR == UDT::recvfile(sock->sock, fstrm, offset, size, block))
                RETURN_UDT_RUNTIME_ERROR;
        
        return Py_BuildValue("i", 0);
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
        {
                "connect",
                (PyCFunction)pyudt4_connect,
                METH_VARARGS,
                ""
        },
        {
                "close",
                (PyCFunction)pyudt4_close,
                METH_VARARGS,
                ""
        },
        {
                "getpeername",
                (PyCFunction)pyudt4_getpeername,
                METH_VARARGS,
                ""
        },
        {
                "getsockname",
                (PyCFunction)pyudt4_getsockname,
                METH_VARARGS,
                ""
        },
        {
                "setsockopt",
                (PyCFunction)pyudt4_setsockopt,
                METH_VARARGS,
                ""
        },
        {
                "getsockopt",
                (PyCFunction)pyudt4_getsockopt,
                METH_VARARGS,
                ""
        },
        {
                "send",
                (PyCFunction)pyudt4_send,
                METH_VARARGS,
                ""
        },
        {
                "recv",
                (PyCFunction)pyudt4_recv,
                METH_VARARGS,
                ""
        },
        {
                "sendmsg",
                (PyCFunction)pyudt4_sendmsg,
                METH_VARARGS,
                ""
        },
        {
                "recvmsg",
                (PyCFunction)pyudt4_recvmsg,
                METH_VARARGS,
                ""
        },
        {
                "sendfile",
                (PyCFunction)pyudt4_sendfile,
                METH_VARARGS,
                ""
        },
        {
                "recvfile",
                (PyCFunction)pyudt4_recvfile,
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
        PyObject *m;
        
        /* init module */
        if (
            PyType_Ready(&pyudt4_socket_type   ) < 0 ||
            PyType_Ready(&pyudt4_exception_type) < 0 ||
            PyType_Ready(&pyudt4_epoll_type    ) < 0
            )
                return;
        
        if ((m = Py_InitModule3("udt4", pyudt4_module_methods, 
                                "Python extension for UDT4"
                                )) == 0x0)
                return;
        
        /* add exception */
        pyudt4_exception_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_exception_type);
        PyModule_AddObject(
                        m, "UDTException", (PyObject*) &pyudt4_exception_type
                        );

        /* add socket objects */
        pyudt4_socket_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_socket_type);
        PyModule_AddObject(
                        m, "UDTSOCKET", (PyObject*) &pyudt4_socket_type
                        );

        /* add epoll objects */
        pyudt4_epoll_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_epoll_type);
        PyModule_AddObject(
                        m, "UDTepoll" , (PyObject*) &pyudt4_epoll_type
                        );

        /* sockoption enums */
        if (
            PyModule_AddIntConstant(m, "UDT_MSS"       , UDT_MSS       ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_SNDSYN"    , UDT_SNDSYN    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RCVSYN"    , UDT_RCVSYN    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_FC"        , UDT_FC        ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_SNDBUF"    , UDT_SNDBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RCVBUF"    , UDT_RCVBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_LINGER"    , UDT_LINGER    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_SNDBUF"    , UDT_SNDBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RCVBUF"    , UDT_RCVBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDP_SNDBUF"    , UDP_SNDBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDP_RCVBUF"    , UDP_RCVBUF    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_MAXMSG"    , UDT_MAXMSG    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_MSGTTL"    , UDT_MSGTTL    ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RENDEZVOUS", UDT_RENDEZVOUS) < 0 ||
            PyModule_AddIntConstant(m, "UDT_SNDTIMEO"  , UDT_SNDTIMEO  ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RCVTIMEO"  , UDT_RCVTIMEO  ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_REUSEADDR" , UDT_REUSEADDR ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_MAXBW"     , UDT_MAXBW     ) < 0 
           ) {
                return;
        }
        
        /* exception codes */
        using namespace UDT;
        if (
            PyModule_AddIntConstant(m, "SUCCESS", 
                                ERRORINFO::SUCCESS     ) < 0 ||  
            PyModule_AddIntConstant(m, "ECONNSETUP", 
                                ERRORINFO::ECONNSETUP  ) < 0 || 
            PyModule_AddIntConstant(m, "ENOSERVER" ,       
                                ERRORINFO::ENOSERVER   ) < 0 || 
            PyModule_AddIntConstant(m, "ECONNREJ",       
                                ERRORINFO::ECONNREJ    ) < 0 || 
            PyModule_AddIntConstant(m, "ESOCKFAIL",       
                                ERRORINFO::ESOCKFAIL   ) < 0 || 
            PyModule_AddIntConstant(m, "ESECFAIL",       
                                ERRORINFO::ESECFAIL    ) < 0 || 
            PyModule_AddIntConstant(m, "ECONNFAIL",       
                                ERRORINFO::ECONNFAIL   ) < 0 || 
            PyModule_AddIntConstant(m, "ECONNLOST",       
                                ERRORINFO::ECONNLOST   ) < 0 || 
            PyModule_AddIntConstant(m, "ENOCONN",       
                                ERRORINFO::ENOCONN     ) < 0 || 
            PyModule_AddIntConstant(m, "ERESOURCE",       
                                ERRORINFO::ERESOURCE   ) < 0 || 
            PyModule_AddIntConstant(m, "ETHREAD",       
                                ERRORINFO::ETHREAD     ) < 0 || 
            PyModule_AddIntConstant(m, "ENOBUF",       
                                ERRORINFO::ENOBUF      ) < 0 || 
            PyModule_AddIntConstant(m, "EFILE",       
                                ERRORINFO::EFILE       ) < 0 || 
            PyModule_AddIntConstant(m, "EINVRDOFF",       
                                ERRORINFO::EINVRDOFF   ) < 0 || 
            PyModule_AddIntConstant(m, "ERDPERM",       
                                ERRORINFO::ERDPERM     ) < 0 || 
            PyModule_AddIntConstant(m, "EINVWROFF",       
                                ERRORINFO::EINVWROFF   ) < 0 || 
            PyModule_AddIntConstant(m, "EWRPERM",       
                                ERRORINFO::EWRPERM     ) < 0 || 
            PyModule_AddIntConstant(m, "EINVOP",       
                                ERRORINFO::EINVOP      ) < 0 || 
            PyModule_AddIntConstant(m, "EBOUNDSOCK",       
                                ERRORINFO::EBOUNDSOCK  ) < 0 || 
            PyModule_AddIntConstant(m, "ECONNSOCK",       
                                ERRORINFO::ECONNSOCK   ) < 0 || 
            PyModule_AddIntConstant(m, "EINVPARAM",       
                                ERRORINFO::EINVPARAM   ) < 0 || 
            PyModule_AddIntConstant(m, "EINVSOCK",      
                                ERRORINFO::EINVSOCK    ) < 0 || 
            PyModule_AddIntConstant(m, "EUNBOUNDSOCK", 
                                ERRORINFO::EUNBOUNDSOCK) < 0 || 
            PyModule_AddIntConstant(m, "ENOLISTEN",       
                                ERRORINFO::ENOLISTEN   ) < 0 || 
            PyModule_AddIntConstant(m, "ERDVNOSERV",       
                                ERRORINFO::ERDVNOSERV  ) < 0 || 
            PyModule_AddIntConstant(m, "ERDVUNBOUND",       
                                ERRORINFO::ERDVUNBOUND ) < 0 || 
            PyModule_AddIntConstant(m, "ESTREAMILL",       
                                ERRORINFO::ESTREAMILL  ) < 0 || 
            PyModule_AddIntConstant(m, "EDGRAMILL",       
                                ERRORINFO::EDGRAMILL   ) < 0 || 
            PyModule_AddIntConstant(m, "EDUPLISTEN",      
                                ERRORINFO::EDUPLISTEN  ) < 0 || 
            PyModule_AddIntConstant(m, "ELARGEMSG",       
                                ERRORINFO::ELARGEMSG   ) < 0 || 
            PyModule_AddIntConstant(m, "EINVPOLLID",       
                                ERRORINFO::EINVPOLLID  ) < 0 || 
            PyModule_AddIntConstant(m, "EASYNCFAIL",       
                                ERRORINFO::EASYNCFAIL  ) < 0 || 
            PyModule_AddIntConstant(m, "EASYNCSND",       
                                ERRORINFO::EASYNCSND   ) < 0 || 
            PyModule_AddIntConstant(m, "EASYNCRCV",       
                                ERRORINFO::EASYNCRCV   ) < 0 || 
            PyModule_AddIntConstant(m, "EPEERERR",       
                                ERRORINFO::EPEERERR    ) < 0 || 
            PyModule_AddIntConstant(m, "EUNKNOWN",             
                                ERRORINFO::EUNKNOWN    ) < 0 
           ) {
                return ;
        }
}

#ifdef __cplusplus
}
#endif 
