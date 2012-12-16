#include "py-udt4-exception.hh"

#include <udt> 

#ifdef __cplusplus
extern "C" {
#endif 

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


PyTypeObject*
initpyudt4_exception_type(PyObject *module)
{
        if (PyType_Ready(&pyudt4_exception_type) < 0)
                return 0x0;

        pyudt4_exception_type.tp_new = PyType_GenericNew;
        Py_INCREF(&pyudt4_exception_type);
        PyModule_AddObject(
                module, "UDTException", (PyObject*) &pyudt4_exception_type
                );

        return (PyTypeObject*) &pyudt4_exception_type;
}


#ifdef __cplusplus
}
#endif 
