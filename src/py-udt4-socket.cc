/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4:        GPLv3 
                        libudt4:        BSD 
        
        12/16/2012      Initial stub 
        12/23/2012      Verified and cleaned for release 1.0  
        */

#include "py-udt4-socket.hh" 

#ifdef __cplusplus
extern "C" {
#endif 

static PyObject*
pyudt4_socket_init(PyTypeObject *type) 
{
        pyudt4_socket_obj *self = (pyudt4_socket_obj*)type->tp_alloc(type, 0);

        self->sock     = 0;
        self->domain   = 0;
        self->type     = 0;
        self->protocol = 0;
        self->valid    = 0;

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
{       return Py_BuildValue("i", self->domain);}


static PyObject*
pyudt4_socket_get_type(pyudt4_socket_obj *self)
{       return Py_BuildValue("i", self->type);}


static PyObject*
pyudt4_socket_get_protocol(pyudt4_socket_obj *self)
{       return Py_BuildValue("i", self->protocol);}


static PyObject*
pyudt4_socket_get_udtsocket(pyudt4_socket_obj *self)
{       return Py_BuildValue("i", self->sock);     }
        
        
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
                (char*) "UDTSOCKET",
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
    0,                                          /* tp_new               */
};


PyTypeObject*
initpyudt4_socket_type(PyObject *module)
{
        if (PyType_Ready(&pyudt4_socket_type) < 0)
                return 0x0;

        pyudt4_socket_type.tp_new = PyType_GenericNew;
       
        Py_INCREF(&pyudt4_socket_type);
        
        PyModule_AddObject(
                module, "UDTSOCKET", (PyObject*) &pyudt4_socket_type
                );

        return (PyTypeObject*) &pyudt4_socket_type;
}

#ifdef __cplusplus
}
#endif 
