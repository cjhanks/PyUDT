/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4 is GPLv3, however it must be linked against UDT4
                        libraries to be of use.  UDT4 license is below.
        @date           12/16/2012 

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

#include "py-udt4-epoll.hh"

#include <set>

#include "py-udt4.hh"

#ifdef __cplusplus
extern "C" {
#endif 

static PyObject *pyudt4_exception_obj = 0x0; 


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
        if (0x0 == flag || flag & 0x1) {
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

        return Py_BuildValue("i",
                        self->readfds .count(sock->sock)  ? 0x1 : 0x0 |
                        self->writefds.count(sock->sock)  ? 0x2 : 0x0 |
                        self->errfds  .count(sock->sock)  ? 0x8 : 0x0 
                        );
}


static PyObject*
pyudt4_epoll_add_ssock(pyudt4_epoll_obj *self, PyObject *args) 
{
        SYSSOCKET sock;
        int       flag = 0x0;

        if (!PyArg_ParseTuple(args, "i|i", &sock, &flag)) {
                PyErr_SetString(PyExc_TypeError,
                        "Invalid ssock"
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
        
        /* decrease the ref for each mapper */
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
        
        return Py_BuildValue("i",
                        self->readfds .count(sock->sock)  ? 0x1 : 0x0 |
                        self->writefds.count(sock->sock)  ? 0x2 : 0x0 |
                        self->errfds  .count(sock->sock)  ? 0x8 : 0x0 
                        );
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
        PyObject *do_sread  = Py_False, 
                 *do_swrite = Py_False;
        
        if (!PyArg_ParseTuple(args, "OOl|OO", &do_uread, &do_uwrite, &timeo, 
                              &do_sread, &do_swrite)) {
                PyErr_SetString(PyExc_TypeError,
                        "invalid epoll args"
                        );

                return 0x0;
        }

        struct {
                std::set<UDTSOCKET> read;
                std::set<UDTSOCKET> write;
        } usock = {
                std::set<UDTSOCKET>(),
                std::set<UDTSOCKET>()
        };

        
        struct {
                std::set<SYSSOCKET> read;
                std::set<SYSSOCKET> write;
        } ssock = {
                std::set<SYSSOCKET>(),
                std::set<SYSSOCKET>()
        };
        
        int rc;
        
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::epoll_wait(self->eid,
                        do_uread  == Py_True ? &usock.read  : 0x0,
                        do_uwrite == Py_True ? &usock.write : 0x0,
                        timeo,
                        do_sread  == Py_True ? &ssock.read  : 0x0,
                        do_swrite == Py_True ? &ssock.write : 0x0
                        );
        Py_END_ALLOW_THREADS;

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
        
        for (std::set<UDTSOCKET>::iterator i = usock.write.begin();
             i != usock.write.end(); ++i) {
                pyudt4_socket_obj *sock = self->writefds[*i];
                Py_XINCREF(sock);
                PySet_Add(uset.write, (PyObject*) sock); 
        }

        /* SYSSOCKET sets */
        for (std::set<SYSSOCKET>::iterator i = ssock.read.begin();
             i != ssock.read.end(); ++i) {
                PySet_Add(sset.read ,  Py_BuildValue("i", *i));
        }
        
        for (std::set<SYSSOCKET>::iterator i = ssock.write.begin();
             i != ssock.write.end(); ++i) {
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
                ":param:  socket        Socket to add   \n"
                ":type:   socket        udt4.UDTSOCKET  \n"
                "\n"
                ":param:  flags           \n"
                ":type:   int()           \n"
                "\n"
                "Add a UDT socket to the EPOLL instance.  The flags can be 0x1\n"
                "(read), 0x4 (write) 0x8 (error)                              \n"
                "\n"
                ":return:       Value of remaining watches : int()            \n"
             
        },
        {
                "add_ssock",
                (PyCFunction)pyudt4_epoll_add_ssock,
                METH_VARARGS,
                ":param:  socket        Socket to add   \n"
                ":type:   socket        int()           \n"
                "\n"
                ":param:  flags           \n"
                ":type:   int()           \n"
                "\n"
                "Add a System socket (UDP, TCP or other), flags are ignored. \n"
                "\n"
                ":return:       PyNONE  \n"
             
        },
        {
                "remove_usock",
                (PyCFunction)pyudt4_epoll_remove_usock,
                METH_VARARGS,
                ":param:  socket        Socket to add   \n"
                ":type:   socket        udt4.UDTSOCKET  \n"
                "\n"
                ":param:  flags           \n"
                ":type:   int()           \n"
                "\n"
                "Remove a watched UDT socket to the EPOLL instance.  The flags\n"
                "can be 0x1 (read), 0x4 (write) 0x8 (error).                  \n"
                "\n"
                ":return:       Value of remaining watches : int()            \n"
            
        },
        {
                "remove_ssock",
                (PyCFunction)pyudt4_epoll_remove_ssock,
                METH_VARARGS,
                ":param:  socket        Socket to add   \n"
                ":type:   socket        udt4.UDTSOCKET  \n"
                "\n"
                ":param:  flags           \n"
                ":type:   int()           \n"
                "\n"
                "Remove a System socket (UDP, TCP or other), flags are ignored.\n"
                "\n"
                ":return:       PyNONE  \n"
        },
        {
                "wait",
                (PyCFunction)pyudt4_epoll_wait,
                METH_VARARGS,
                ":param  do_uread       Return waiting UDT Read sockets? \n"
                ":type   do_uread       bool()\n"
                "\n"
                ":param  do_uwrite      Return waiting UDT Write sockets? \n"
                ":type   do_uwrite      bool()\n"
                "\n"
                ":param  wait           EPOLL timeout                    \n"
                ":type   wait           int() \n"
                "\n"
                ":param  do_sread       Return waiting UDT Read sockets? \n"
                ":type   do_sread       bool() default = False\n"
                "\n"
                ":param  do_swrite      Return waiting UDT Read sockets? \n"
                ":type   do_swrite      bool() default = False\n"
                "\n"
                ":return: tuple(frozenset(read_udt_sockets) ,\n"
                "               frozenset(write_udt_sockets),\n"
                "               frozenset(write_sys_sockets),\n"
                "               frozenset(write_sys_sockets))\n"
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


PyTypeObject*
initpyudt4_epoll_type(PyObject *module, PyObject *exception_type)
{
        if (PyType_Ready(&pyudt4_epoll_type) < 0)
                return 0x0;

        pyudt4_exception_obj = exception_type; 

        pyudt4_epoll_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_epoll_type);
        PyModule_AddObject(
                module, "UDTepoll", (PyObject*) &pyudt4_epoll_type
                );

        return (PyTypeObject*) &pyudt4_epoll_type;        
}

#ifdef __cplusplus
}
#endif 
