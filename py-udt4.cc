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

#include "py-udt4.hh"

#define DEBUG(_msg_) fprintf(stderr, _msg_) 

#define RETURN_UDT_RUNTIME_ERROR \
        do {\
                PyErr_SetString(\
                        PyExc_RuntimeError,\
                        UDT::getlasterror().getErrorMessage()\
                        );\
                \
                return 0x0;\
        } while (0)



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

static PyObject*
pyudt4_socket_debug(PyObject *py_self)
{
        pyudt4_socket_obj *self = (pyudt4_socket_obj*) py_self;

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
    pyudt4_socket_methods,                      /* tp_methods           */
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
                RETURN_UDT_RUNTIME_ERROR;
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
                
                return 0x0;
        } else {
                /* TODO: assert some sanity in the addres/port */
                sock = (pyudt4_socket_obj*) sock_obj;
        }
        
        /*
        if (inet_pton(sock->domain, address, &sock_addr.sin_addr) == 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid address (invalid inet_pton(...)"
                        );
                
                return 0x0;
        }
        */

        sock_addr.sin_family      = sock->domain;
        sock_addr.sin_port        = htons(port);
        sock_addr.sin_addr.s_addr = INADDR_ANY; 

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
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        int backlog = -1;

        if (!PyArg_ParseTuple(args, "Oi", &sock_obj, &backlog)) {
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
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
        
        if (UDT::listen(sock->sock, backlog)) {
                RETURN_UDT_RUNTIME_ERROR;
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
                
                return 0x0;
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
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

        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        char *host = 0x0;
        int   port = -1;;
        
        if (!PyArg_ParseTuple(args, "Osi", &sock_obj, &host, &port)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );

                return 0x0; 
        } else {
                /* TODO assert some sanity in the address/port */
                sock = (pyudt4_socket_obj*) sock_obj;
        }
       
        sockaddr_in serv;
        assert(sock->domain == AF_INET);
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
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }

        return Py_BuildValue("i", UDT::close(sock->sock));
} 

/*
static PyObject*
pyudt4_getpeername(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
        

} 


static PyObject*
pyudt4_getsockname(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
} 
*/

static PyObject*
pyudt4_setsockopt(PyObject *py_self, PyObject *args)
{
        int rc; 

        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        int          optname_i;
        UDT::SOCKOPT optname;
        long         optval_o;
         
        if (!PyArg_ParseTuple(args, "Oil", &sock_obj, &optname_i, &optval_o)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [OPTION] [OPTVALUE]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock    = (pyudt4_socket_obj*) sock_obj;
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
               
                if (!PyArg_ParseTuple(args, "OiO", &sock_obj, &optname_i, 
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

                if (!PyArg_ParseTuple(args, "Oiii", &sock_obj, 0, 
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

        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        int          optname_i;
        UDT::SOCKOPT optname;
        int          len;



        if (!PyArg_ParseTuple(args, "Oi", &sock_obj, &optname_i)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        } else {
                sock    = (pyudt4_socket_obj*) sock_obj;
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

        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;  /* true buffer length    */
        int   pref_len; /* passed in length size */

        if (!PyArg_ParseTuple(args, "Os#i", &sock_obj, &buf, &buf_len, 
                              &pref_len)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
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

                sock = (pyudt4_socket_obj*) sock_obj;
        }
        

        if (pref_len > buf_len) {
                rc = UDT::send(sock->sock, buf, buf_len, 0);
                
                if (UDT::ERROR == rc) 
                        RETURN_UDT_RUNTIME_ERROR;

                /* send remainder */
                memset(buf, '\0', pref_len - buf_len);
                
                rc = UDT::send(sock->sock, buf, pref_len - buf_len, 0);
                
                if (UDT::ERROR == rc) 
                        RETURN_UDT_RUNTIME_ERROR;
                 
                return Py_BuildValue("i", rc);
        } else {
                return Py_BuildValue("i", UDT::send(sock->sock, buf, buf_len, 
                                                    0));
        }
} 


static PyObject*
pyudt4_recv(PyObject *py_self, PyObject *args)
{
        int rc; 

        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;
        
        char *buf;
        int   buf_len;

        if (!PyArg_ParseTuple(args, "Oi", &sock_obj, &buf_len)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [INT]" 
                        );
                
                return 0x0;
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }

        buf = (char*)malloc(sizeof(char) * buf_len);

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
                free(buf);
                RETURN_UDT_RUNTIME_ERROR;
        }

        PyObject *ret = Py_BuildValue("s#", buf, buf_len);
        free(buf);

        return ret;
} 

/*
static PyObject*
pyudt4_sendmsg(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
} 


static PyObject*
pyudt4_recvmsg(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
} 


static PyObject*
pyudt4_sendfile(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
} 


static PyObject*
pyudt4_recvfile(PyObject *py_self, PyObject *args)
{
        PyObject *sock_obj      = 0x0;
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock_obj)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return Py_BuildValue("i", 1);
        } else {
                sock = (pyudt4_socket_obj*) sock_obj;
        }
} 
*/


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
        /*
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
        */
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
        /*
        {
                "sendmsg",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                ""
        },
        {
                "recvmsg",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                ""
        },
        {
                "sendfile",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                ""
        },
        {
                "recvfile",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                ""
        },
        */
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
        if (PyType_Ready(&pyudt4_socket_type) < 0)
                return;
        
        if ((m = Py_InitModule3("udt4", pyudt4_module_methods, 
                                "Python extension for UDT4"
                                )) == 0x0)
                return;
        
        /* add socket objects */
        pyudt4_socket_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_socket_type);
        PyModule_AddObject(m, "UDTSOCKET", (PyObject*) &pyudt4_socket_type);


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
}

#ifdef __cplusplus
}
#endif 
