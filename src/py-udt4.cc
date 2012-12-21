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
#include "py-udt4.hh"

#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h> 

#include <fstream> 

#include "py-udt4-epoll.hh"
#include "py-udt4-socket.hh" 

#ifdef __cplusplus
extern "C" {
#endif 

static PyTypeObject *pyudt4_socket_type    = 0x0;
static PyObject     *pyudt4_exception_obj = 0x0; 
static PyTypeObject *pyudt4_epoll_type     = 0x0; 

/* -------------------------------------------------------------------------- */
static PyObject*
pyudt4_pyudt4_version(PyObject *py_self)
{
        return Py_BuildValue(
                        "ii", MAJOR_VERSION, MINOR_VERSION
                        );
}

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
        
        sock = (pyudt4_socket_obj*)_PyObject_New(pyudt4_socket_type);

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

        sock_addr.sin_family = sock->domain;
        sock_addr.sin_port   = htons(port);

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
        
        UDTSOCKET client;
        
        Py_BEGIN_ALLOW_THREADS;
        client = UDT::accept(sock->sock, (sockaddr*) &client_stg, &addr_len);
        Py_END_ALLOW_THREADS;

        if (UDT::INVALID_SOCK == client) {
                RETURN_UDT_RUNTIME_ERROR;
        } 

        /* create a socket */
        pyudt4_socket_obj *client_sock = 
                (pyudt4_socket_obj*)_PyObject_New(pyudt4_socket_type);
        
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
                len = sizeof(optval);

                if (UDT::ERROR == (rc = UDT::getsockopt(sock->sock, 0, optname,
                                                        &optval, &len))) {
                        RETURN_UDT_RUNTIME_ERROR;
                } else {
                        return (optval) ? Py_True : Py_False;
                }
        }

        case UDT_LINGER: {
                linger optval;
                len = sizeof(optval);

                if (UDT::ERROR == (rc = UDT::getsockopt(sock->sock, 0, optname,
                                                        &optval, &len))) {
                        RETURN_UDT_RUNTIME_ERROR;
                } else {
                        return Py_BuildValue(
                                        "ii", optval.l_onoff, optval.l_linger
                                        );
                }
        }

        case UDT_STATE:
        case UDT_EVENT:
        case UDT_SNDDATA:
        case UDT_RCVDATA: {
                int optval;
                len = sizeof(optval);
                
                if (UDT::ERROR == (rc = UDT::getsockopt(sock->sock, 0, optname,
                                                        &optval, &len))) {
                        RETURN_UDT_RUNTIME_ERROR; 
                } else {
                        return Py_BuildValue(
                                        "i", optval
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
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::send(sock->sock, buf, buf_len, 0);
                Py_END_ALLOW_THREADS;

                if (UDT::ERROR == rc) 
                        RETURN_UDT_RUNTIME_ERROR;

                /* send remainder */
                memset(buf, '\0', pref_len - buf_len);
                
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::send(sock->sock, buf, pref_len - buf_len, 0);
                Py_END_ALLOW_THREADS;
        } else {
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::send(sock->sock, buf, buf_len, 0);
                Py_END_ALLOW_THREADS;
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
                
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::sendmsg(sock->sock, new_buf, pref_len, ttl, 
                                  in_order != Py_False);
                Py_END_ALLOW_THREADS;

                PyMem_Free(new_buf);
        } else {
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::sendmsg(sock->sock, buf, buf_len, ttl, 
                                  in_order != Py_False);
                Py_END_ALLOW_THREADS;
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
        
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::recv(sock->sock, buf, buf_len, 0);
        Py_END_ALLOW_THREADS;

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

        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::recvmsg(sock->sock, buf, buf_len);
        Py_END_ALLOW_THREADS;

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

        int rc;
        
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::sendfile(sock->sock, fstrm, offset, size, block);
        Py_END_ALLOW_THREADS; 

        if (UDT::ERROR == rc)
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
        
        int rc;
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::recvfile(sock->sock, fstrm, offset, size, block);
        Py_END_ALLOW_THREADS;
        
        if (UDT::ERROR == rc) 
                RETURN_UDT_RUNTIME_ERROR;
        
        return Py_BuildValue("i", 0);
} 


static PyMethodDef pyudt4_module_methods[] = {
        {
                "pyudt4_version",
                (PyCFunction)pyudt4_pyudt4_version,
                METH_NOARGS,
                ":return: tuple(major, minor)"
        },
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
                "Initialize a socket (domain, type, protocol)           \n"
                ":param domain:         socket domain/family            \n"
                ":type  domain:         int()                           \n"
                "\n"
                ":param type:           socket type                     \n"
                ":type  type:           int()                           \n"
                "\n"
                ":param protocol:       socket protocol                 \n"
                ":type  protocol:       int()                           \n"
                "\n"
                "@return UDTSOCKET class instance"

        },
        {
                "bind",
                (PyCFunction)pyudt4_bind,
                METH_VARARGS,
                "Bind socket to address (host, port)                    \n"
                ":param host:           local host ip to bind to        \n"
                ":type  host:           str()                           \n"
                "\n"
                ":param port:           local isten port to bind to     \n"
                ":type  port:           int()                           \n"
                "\n"
        },
        {
                "listen",
                (PyCFunction)pyudt4_listen,
                METH_VARARGS,
                "Mark socket as passive to accept (backlog)             \n"
                ":param backlog:        maximum size of queue           \n"
                ":type  backlog:        int()                           \n"
                "\n"
                "@return 0 for success                                    "
        },
        {
                "accept",
                (PyCFunction)pyudt4_accept,
                METH_VARARGS,
                "Blocking call.  Currently this call will block permanently  \n" 
                "and EscapeExceptions will not break its execution           \n"
                "(unforunately)                                              \n"
                "\n"
                ":return tuple(UDTSOCKET, str)                               \n"
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
init_udt4()
{
        PyObject *m;
         
        if ((m = Py_InitModule3("_udt4", pyudt4_module_methods, 
                                "Python extension for UDT4"
                                )) == 0x0)
                return;
       
        /* add exception */
        pyudt4_exception_obj = PyErr_NewException(
                                        "udt4.UDTException", 0x0, 0x0
                                        );
        Py_INCREF(pyudt4_exception_obj); 
        PyModule_AddObject(
                        m, "UDTException", pyudt4_exception_obj
                        );

        pyudt4_socket_type   = initpyudt4_socket_type(m);
        pyudt4_epoll_type    = initpyudt4_epoll_type(m, pyudt4_exception_obj);
        
        assert(0x0 != pyudt4_exception_obj);

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
            PyModule_AddIntConstant(m, "UDT_MAXBW"     , UDT_MAXBW     ) < 0 ||

            /* read only's */
            PyModule_AddIntConstant(m, "UDT_STATE"     , UDT_STATE     ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_EVENT"     , UDT_EVENT     ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_SNDDATA"   , UDT_SNDDATA   ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_RCVDATE"   , UDT_RCVDATA   ) < 0 
           ) {
                return;
        }

        /* UDTSTATUS */
        if (
            PyModule_AddIntConstant(m, "UDTSTATUS_INIT"      , INIT      ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_OPENED"    , OPENED    ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_LISTENING" , LISTENING ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_CONNECTED" , CONNECTED ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_CONNECTING", CONNECTING) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_BROKEN"    , BROKEN    ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_CLOSING"   , CLOSING   ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_CLOSED"    , CLOSED    ) < 0 ||
            PyModule_AddIntConstant(m, "UDTSTATUS_NONEXIST"  , NONEXIST  ) < 0 
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
