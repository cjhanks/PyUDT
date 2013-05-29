/**
        @author         Christopher J. Hanks    <develop@cjhanks.name>
        @license        py-udt4:        GPLv3 
                        libudt4:        BSD 
        
        12/15/2012      Initial stub 
        12/23/2012      Verified and cleaned for release 1.0  
        */


#include "py-udt4.hh"

#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#include <fstream> 

#include "py-udt4-epoll.hh"
#include "py-udt4-socket.hh" 


#ifdef __cplusplus
extern "C" {
#endif 

static PyTypeObject *pyudt4_socket_type    = 0x0;
static PyObject     *pyudt4_exception_obj  = 0x0; 
static PyTypeObject *pyudt4_epoll_type     = 0x0; 

/* -------------------------------------------------------------------------- */
typedef struct __pyudt4_perfmon_obj {
        PyObject_HEAD;
        UDT::TRACEINFO trace;
} pyudt4_perfmon_obj;


static PyObject*
pyudt4_perfmon_msTimeStamp(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("l", self->trace.msTimeStamp);     }

static PyObject*
pyudt4_perfmon_pktSentTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("l", self->trace.pktSentTotal);    }

static PyObject*
pyudt4_perfmon_pktRecvTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("l", self->trace.pktRecvTotal);    }

static PyObject*
pyudt4_perfmon_pktSndLossTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSndLossTotal); }

static PyObject*
pyudt4_perfmon_pktRcvLossTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRcvLossTotal); }

static PyObject*
pyudt4_perfmon_pktRetransTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRetransTotal); }

static PyObject*
pyudt4_perfmon_pktSentACKTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSentACKTotal); }

static PyObject*
pyudt4_perfmon_pktRecvACKTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRecvACKTotal); }

static PyObject*
pyudt4_perfmon_pktSentNAKTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSentNAKTotal); }

static PyObject*
pyudt4_perfmon_pktRecvNAKTotal(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRecvNAKTotal); }

static PyObject*
pyudt4_perfmon_pktSent(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("l", self->trace.pktSent);         }

static PyObject*
pyudt4_perfmon_pktRecv(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("l", self->trace.pktRecv);         }

static PyObject*
pyudt4_perfmon_pktSndLoss(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSndLoss);      }

static PyObject*
pyudt4_perfmon_pktRcvLoss(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRcvLoss);      }

static PyObject*
pyudt4_perfmon_pktRetrans(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRetrans);      }

static PyObject*
pyudt4_perfmon_pktSentACK(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSentACK);      }

static PyObject*
pyudt4_perfmon_pktRecvACK(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRecvACK);      }

static PyObject*
pyudt4_perfmon_pktSentNAK(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktSentNAK);      }

static PyObject*
pyudt4_perfmon_pktRecvNAK(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktRecvNAK);      }

static PyObject*
pyudt4_perfmon_mbpsSendRate(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("f", self->trace.mbpsSendRate);    }

static PyObject*
pyudt4_perfmon_mbpsRecvRate(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("f", self->trace.mbpsRecvRate);    }

static PyObject*
pyudt4_perfmon_usPktSndPeriod(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("d", self->trace.usPktSndPeriod);  }

static PyObject*
pyudt4_perfmon_pktFlowWindow(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktFlowWindow);   }

static PyObject*
pyudt4_perfmon_pktCongestionWindow(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktCongestionWindow);}

static PyObject*
pyudt4_perfmon_pktFlightSize(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.pktFlightSize);   }

static PyObject*
pyudt4_perfmon_msRTT(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("f", self->trace.msRTT);           }

static PyObject*
pyudt4_perfmon_mbpsBandwidth(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("f", self->trace.mbpsBandwidth);   }

static PyObject*
pyudt4_perfmon_byteAvailSndBuf(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.byteAvailSndBuf); }

static PyObject*
pyudt4_perfmon_byteAvailRcvBuf(pyudt4_perfmon_obj *self)
{       return Py_BuildValue("i", self->trace.byteAvailRcvBuf); }


static PyGetSetDef pyudt4_perfmon_getset[] = {
        {
                (char*)  "msTimeStamp",
                (getter) pyudt4_perfmon_msTimeStamp,
                (setter) 0x0,
                (char*)  ""
        },
        {
                (char*)  "pktSentTotal",
                (getter) pyudt4_perfmon_pktSentTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecvTotal",
                (getter) pyudt4_perfmon_pktRecvTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSndLossTotal",
                (getter) pyudt4_perfmon_pktSndLossTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRcvLossTotal",
                (getter) pyudt4_perfmon_pktRcvLossTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRetransTotal",
                (getter) pyudt4_perfmon_pktRetransTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSentACKTotal",
                (getter) pyudt4_perfmon_pktSentACKTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecvACKTotal",
                (getter) pyudt4_perfmon_pktRecvACKTotal, 
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSentNAKTotal",
                (getter) pyudt4_perfmon_pktSentNAKTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecvNAKTotal",
                (getter) pyudt4_perfmon_pktRecvNAKTotal,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSent",
                (getter) pyudt4_perfmon_pktSent,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecv",
                (getter) pyudt4_perfmon_pktRecv,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSndLoss",
                (getter) pyudt4_perfmon_pktSndLoss,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRcvLoss",
                (getter) pyudt4_perfmon_pktRcvLoss, 
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRetrans",
                (getter) pyudt4_perfmon_pktRetrans, 
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSentACK",
                (getter) pyudt4_perfmon_pktSentACK,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecvACK",
                (getter) pyudt4_perfmon_pktRecvACK,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktSentNAK",
                (getter) pyudt4_perfmon_pktSentNAK,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktRecvNAK",
                (getter) pyudt4_perfmon_pktRecvNAK,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "mbpsSendRate",
                (getter) pyudt4_perfmon_mbpsSendRate,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "mbpsRecvRate",
                (getter) pyudt4_perfmon_mbpsRecvRate,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "usPktSndPeriod",
                (getter) pyudt4_perfmon_usPktSndPeriod,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktFlowWindow",
                (getter) pyudt4_perfmon_pktFlowWindow,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktCongestionWindow",
                (getter) pyudt4_perfmon_pktCongestionWindow,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "pktFlightSize",
                (getter) pyudt4_perfmon_pktFlightSize,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "msRTT",
                (getter) pyudt4_perfmon_msRTT,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "mbpsBandwidth",
                (getter) pyudt4_perfmon_mbpsBandwidth,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "byteAvailSndBuf",
                (getter) pyudt4_perfmon_byteAvailSndBuf,
                (setter) 0x0,
                (char*)  "" 
        },
        {
                (char*)  "byteAvailRcvBuf",
                (getter) pyudt4_perfmon_byteAvailRcvBuf,
                (setter) 0x0,
                (char*)  "" 
        },
        { 0x0 }
};


static PyTypeObject pyudt4_perfmon_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /* ob_size              */
    "TRACEINFO",                                  /* tp_name              */
    sizeof(pyudt4_perfmon_obj),                 /* tp_basicsize         */
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
    "UDT::TRACEINFO",                           /* tp_doc               */
    0,                                          /* tp_traverse          */
    0,                                          /* tp_clear             */
    0,                                          /* tp_richcompare       */
    0,                                          /* tp_weaklistoffset    */
    0,                                          /* tp_iter              */
    0,                                          /* tp_iternext          */
    0,                                          /* tp_methods           */
    0,                                          /* tp_members           */
    pyudt4_perfmon_getset,                      /* tp_getset            */
    0,                                          /* tp_base              */
    0,                                          /* tp_dict              */
    0,                                          /* tp_descr_get         */
    0,                                          /* tp_descr_set         */
    0,                                          /* tp_dictoffset        */
    0,                                          /* tp_init              */
    0,                                          /* tp_alloc             */
    0,                                          /* tp_new               */
};


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
                        "arguments: [domain => AF_INET] [type] [protocol]"
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
                
                Py_DECREF(sock);
                return 0x0;
        }

        /* initialize socket */
        sock->sock = UDT::socket(domain, type, protocol);
        
        if (UDT::ERROR == sock->sock) {
                Py_DECREF(sock);
                RETURN_UDT_RUNTIME_ERROR;
        }

        return (PyObject*) sock;
}


static PyObject* 
pyudt4_bind_to_udp(PyObject *py_self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0;
        int udp_sock = -1;

        if (!PyArg_ParseTuple(args, "Oi", &sock, &udp_sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [UDPSOCKET int()]"
                        );

                return 0x0; 
        }

        if (UDT::ERROR == UDT::bind2(sock->sock, udp_sock))
                RETURN_UDT_RUNTIME_ERROR;

        Py_RETURN_NONE;
}


static PyObject*
pyudt4_bind(PyObject *py_self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0;
        
        sockaddr_in sock_addr;
        char *address = 0x0;
        int   port    = -1 ;
       
        if (!PyArg_ParseTuple(args, "Osi", &sock, &address, &port)) {
                /* try bind_to_udp */
                PyObject *tret = pyudt4_bind_to_udp(py_self, args);
                
                if (0x0 != tret)
                        return tret; 
                
                /* nope, just move on with error */
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [address] [port]"
                        );
                
                return 0x0;
        }
        
        if (inet_pton(sock->domain, address, &sock_addr.sin_addr) == 0) {
                PyErr_SetString(
                        PyExc_ValueError,
                        "invalid address failed inet_pton(...)"
                        );
                
                return 0x0;
        }
         
        sock_addr.sin_family = sock->domain;
        sock_addr.sin_port   = htons(port);
        
        memset(&sock_addr.sin_zero, '\0', sizeof(sock_addr.sin_zero));
        
        int rc;
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::bind(sock->sock, (sockaddr*) &sock_addr, sizeof(sock_addr));
        Py_END_ALLOW_THREADS; 
        
        if (UDT::ERROR == rc) 
                RETURN_UDT_RUNTIME_ERROR;
        
        sock->valid = 1;

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
                PyErr_SetString(PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        }

        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);
        
        UDTSOCKET client;
        
        Py_BEGIN_ALLOW_THREADS;
        client = UDT::accept(sock->sock, (sockaddr*) &client_stg, &addr_len);
        Py_END_ALLOW_THREADS;

        if (UDT::INVALID_SOCK == client) 
                RETURN_UDT_RUNTIME_ERROR;
        
        /* create a socket */
        pyudt4_socket_obj *client_sock = 
                (pyudt4_socket_obj*)_PyObject_New(pyudt4_socket_type);
        
        client_sock->sock     = client;
        client_sock->domain   = sock->domain;
        client_sock->type     = sock->type;
        client_sock->protocol = sock->protocol;
        client_sock->valid    = 1;
        
        char client_host[NI_MAXHOST];
        char client_srvc[NI_MAXSERV];

        memset(client_host, '\0', sizeof(client_host));
        memset(client_srvc, '\0', sizeof(client_srvc));
        
        /* get hostname to return back */
        if (getnameinfo((sockaddr*) &client_stg, addr_len, 
                        client_host, sizeof(client_host) , 
                        client_srvc, sizeof(client_srvc) , 
                        NI_NUMERICHOST | NI_NUMERICSERV  )) {
                PyErr_SetString(
                        PyExc_RuntimeError, "Failed to get the client host info"
                        );

                return 0x0; 
        }
       
        return Py_BuildValue("O(si)", (PyObject*) client_sock, client_host,
                             client_stg.sin_port);
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
                        "arguments: [UDTSOCKET] [PORT]"
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
        
        int rc; 
        
        Py_BEGIN_ALLOW_THREADS;
        rc = UDT::connect(sock->sock, (sockaddr*) &serv, sizeof(serv));
        Py_END_ALLOW_THREADS; 

        if (UDT::ERROR == rc) 
                RETURN_UDT_RUNTIME_ERROR;
        
        sock->valid = 1;
        
        return Py_BuildValue("i", 0);
}


static PyObject*
pyudt4_close(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        } 
        
        /* closing on invalid socket causes SIGSEGV */
        if (sock->valid) {
                int rc;
                Py_BEGIN_ALLOW_THREADS;
                rc = UDT::close(sock->sock);
                Py_END_ALLOW_THREADS; 
                return Py_BuildValue("i", rc);
        } else {
                return Py_BuildValue("i", -1);
        }
} 


static PyObject*
pyudt4_getpeername(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(PyExc_TypeError,
                        "arguments: [UDTSOCKET]"
                        );
                
                return 0x0;
        }
        
        sockaddr_in client_stg;
        int addr_len = sizeof(client_stg);

        if (UDT::ERROR == UDT::getpeername(sock->sock, (sockaddr*) &client_stg,
                                           &addr_len))
                RETURN_UDT_RUNTIME_ERROR;
        

        char client_host[NI_MAXHOST];
        char client_srvc[NI_MAXSERV];

        memset(client_host, '\0', sizeof(client_host));
        memset(client_srvc, '\0', sizeof(client_srvc));

        if (getnameinfo((sockaddr*) &client_stg, addr_len, 
                        client_host, sizeof(client_host) , 
                        client_srvc, sizeof(client_srvc) ,
                        NI_NUMERICHOST | NI_NUMERICSERV) ) {
                PyErr_SetString(PyExc_RuntimeError,
                        "Failed to getnameinfo(...)"
                        );

                return 0x0;
        }
       
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

        if (getnameinfo((sockaddr*) &client_stg, addr_len, 
                        client_host, sizeof(client_host) , 
                        client_srvc, sizeof(client_srvc) ,
                        NI_NUMERICHOST | NI_NUMERICSERV) ) {
                
                PyErr_SetString(PyExc_RuntimeError,
                        "Failed to getnameinfo(...)"
                        );

                return 0x0;
        }
       
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
        PyObject    *bogus_opt;
         
        if (!PyArg_ParseTuple(args, "OiO", &sock, &optname_i, &bogus_opt)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET] [OPTION]"
                        );
                
                return 0x0;  
        } else {
                optname = (UDT::SOCKOPT) optname_i;
        }
        
        /* switch opt name */
        switch (optname) {
        
        // -- int() 
        case UDT_MSS     :
        case UDT_FC      :
        case UDT_SNDBUF  :
        case UDT_RCVBUF  :
        case UDP_SNDBUF  :
        case UDP_RCVBUF  :
        case UDT_SNDTIMEO:
        case UDT_RCVTIMEO: {
                if (!PyInt_Check(bogus_opt)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "Option value should be of type bool()"
                                );

                        return 0x0;
                }
                
                int optval = PyInt_AsLong(bogus_opt);
                
                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname, 
                                                        &optval, sizeof(int)))
                   ) {
                        RETURN_UDT_RUNTIME_ERROR;
                }

                break;
        }
        
        // -- long() 
        case UDT_MAXBW   : {
                if (!PyLong_Check(bogus_opt)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "Option value should be of type long()"
                                );

                        return 0x0;
                }
                
                long optval = PyInt_AsLong(bogus_opt);
               
                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname, 
                                                        &optval, sizeof(long)))
                   ) {
                        RETURN_UDT_RUNTIME_ERROR;
                }

                break;
        }
        
        // -- bool() 
        case UDT_SNDSYN    :
        case UDT_RCVSYN    :
        case UDT_RENDEZVOUS:
        case UDT_REUSEADDR : {
                if (!PyBool_Check(bogus_opt)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "Option value should be of type bool()"
                                );

                        return 0x0;
                }

                bool optval = (Py_True == bogus_opt);

                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname,
                                                        &optval, sizeof(bool)))
                   ) {
                        RETURN_UDT_RUNTIME_ERROR;
                }

                break;
        }
        
        // -- tuple() 
        case UDT_LINGER: {
                linger optval;
                
                if (! PyTuple_CheckExact(bogus_opt)) {
                        PyErr_SetString(
                                PyExc_TypeError,
                                "Value must be of type tuple() for linger"
                                );

                        return 0x0; 
                } else {
                        optval.l_onoff  = PyInt_AsLong(
                                                 PyTuple_GetItem(bogus_opt, 0)
                                                 );
                        optval.l_linger = PyInt_AsLong(
                                                 PyTuple_GetItem(bogus_opt, 1)
                                                 );
                }

                if (UDT::ERROR == (rc = UDT::setsockopt(sock->sock, 0, optname,
                                                     &optval, sizeof(optval)))
                   ) {
                        RETURN_UDT_RUNTIME_ERROR;
                }

                break;
        }
        
        // -- error 
        default: {
                PyErr_SetString(
                        PyExc_ValueError, "invalid option name to setsockopt()"  
                        );
                
                return 0x0;
        }
        }


        return Py_BuildValue("i", rc);
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
pyudt4_getsockstate(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;

        if (!PyArg_ParseTuple(args, "O", &sock)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "arguments: [UDTSOCKET]" 
                        );
                
                return 0x0;
        }

        return Py_BuildValue("i", UDT::getsockstate(sock->sock));
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
                   udp dgram packets must be sent in a single send for those
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
                        "arguments: [UDTSOCKET] [INT LENGTH]" 
                        );
                
                return 0x0;
        }

        buf = (char*)PyMem_Malloc(sizeof(char) * buf_len);

        if (0x0 == buf) {
                PyErr_SetString(
                        PyExc_MemoryError, "memory allocation error"
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

        PyObject *ret = Py_BuildValue("s#", buf, rc);
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

        PyObject *ret = Py_BuildValue("s#", buf, rc);
        PyMem_Free(buf);

        return ret;
} 

static PyObject*
pyudt4_sendfile(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        PyObject *fobj;
        long  offset;
        long  size;
        int   block = 7320000;
        
        if (!PyArg_ParseTuple(args, "OOll|l", &sock, &fobj, &offset, &size,
                              &block)) {
                PyErr_SetString(PyExc_TypeError,
                        "arguments: [UDTSOCKET] [FILEOBJ] [LONG] [LONG] [INT]"
                        );

                return 0x0;
        } 

        /* acquire the file obj */
        if (!PyFile_Check(fobj)) {
                PyErr_SetString(PyExc_ValueError,
                        "File must be file object"
                        );

                return 0x0;
        }
       
        FILE *fd  = PyFile_AsFile(fobj);
        char *buf = (char*) PyMem_Malloc(block);
        long  cnt = 0;
        long  rc; 
        
        PyFile_IncUseCount((PyFileObject*) fobj);
        Py_BEGIN_ALLOW_THREADS;
        
        while (cnt < size) {
                rc = fread(buf, sizeof(char), 
                           block > (size - cnt) ? block : (size - cnt), fd
                          );
        
                if (0 == rc && feof(fd)) {
                        break;
                } else {
                        rc = UDT::send(sock->sock, buf, rc, 0);

                        if (UDT::ERROR == rc)
                                break; 
                        else 
                                cnt += rc;
                }
        }
        
        Py_END_ALLOW_THREADS; 
        PyFile_DecUseCount((PyFileObject*) fobj);
        
        return Py_BuildValue("l", cnt);
} 


static PyObject*
pyudt4_recvfile(PyObject *py_self, PyObject *args)
{
        pyudt4_socket_obj *sock = 0x0;
        
        PyObject *fobj;
        long  offset;
        long  size;
        int   block = 366000;

        if (!PyArg_ParseTuple(args, "OOll|l", &sock, &fobj, &offset, &size,
                              &block)) {
                PyErr_SetString(PyExc_TypeError,
                        "arguments: [UDTSOCKET] [FILEOBJ] [LONG] [LONG] [INT]"
                        );

                return 0x0;
        }
       
        /* acquire the file obj */
        if (!PyFile_Check(fobj)) {
                PyErr_SetString(PyExc_ValueError,
                        "File must be file object"
                        );

                return 0x0;
        }
        
        FILE *fd  = PyFile_AsFile(fobj);
        char *buf = (char*) PyMem_Malloc(block);
        long cnt  = 0; 
        long rc;

        PyFile_IncUseCount((PyFileObject*) fobj);
        Py_BEGIN_ALLOW_THREADS; 

        while (cnt < size) {
                rc = UDT::recv(sock->sock, buf, block, 0);

                if (UDT::ERROR == rc) {
                        break;
                } else {
                        cnt += fwrite(buf, sizeof(char), rc, fd);
                }
        }
        
        Py_END_ALLOW_THREADS; 
        PyFile_DecUseCount((PyFileObject*) fobj);

        PyMem_Free(buf);

        return Py_BuildValue("l", cnt);
} 


static PyObject* 
pyudt4_perfmon(PyObject *self, PyObject *args) 
{
        pyudt4_socket_obj *sock = 0x0;
        PyObject *clear = Py_True; 

        if (!PyArg_ParseTuple(args, "O|O", &sock, &clear)) {
                PyErr_SetString(
                        PyExc_TypeError,
                        "ERROR HERE__"
                        );

                return 0x0;
        }
        
        pyudt4_perfmon_obj *perf = 
                (pyudt4_perfmon_obj*)  _PyObject_New(&pyudt4_perfmon_type);
         
        if (0x0 == perf) {
                PyErr_SetString(
                        PyExc_MemoryError, 
                        "Failed to allocate UDT::TRACEINFO"
                        );

                return 0x0;
        } 

        if (UDT::ERROR == UDT::perfmon(sock->sock, &perf->trace, 
                                       clear == Py_True)) { 
                Py_DECREF(perf);   
                RETURN_UDT_RUNTIME_ERROR;
        }

        return (PyObject*) perf;
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
                ":return:       UDTSOCKET class instance"

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
                "bind_to_udp",
                (PyCFunction)pyudt4_bind_to_udp,
                METH_VARARGS,
                "Bind an existing socket to a UDP socket" 
        },
        {
                "listen",
                (PyCFunction)pyudt4_listen,
                METH_VARARGS,
                "Mark socket as passive to accept (backlog)             \n"
                ":param backlog:        maximum size of queue           \n"
                ":type  backlog:        int()                           \n"
                "\n"
                ":return:       0 for success                           "
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
                "Connect socket to host, port           \n"
                "\n"
                ":param socket: Socket to act on        \n"
                ":type  socket: UDTSOCKET               \n"
                "\n"
                ":param host:   IP host to connect to   \n"
                ":type  host:   str()                   \n"
                "\n"
                ":param port:   Port to connect to.     \n"
                ":type  port:   int()                   \n"
        },
        {
                "close",
                (PyCFunction)pyudt4_close,
                METH_VARARGS,
                "Close the socket                       \n"
                "\n"
                ":param socket: Socket to act on        \n"
                ":type  socket: UDTSOCKET               \n"
        },
        {
                "getpeername",
                (PyCFunction)pyudt4_getpeername,
                METH_VARARGS,
                "Get the peername information for a connected socket \n" 
                "\n"
                ":param socket: Socket to act on        \n"
                ":type  socket: UDTSOCKET               \n"
                "\n"
                ":return: host, port \n"
                ":type:   str(),int()\n"
        },
        {
                "getsockname",
                (PyCFunction)pyudt4_getsockname,
                METH_VARARGS,
                "Get the socket information for a connected socket \n" 
                "\n"
                ":param socket: Socket to act on        \n"
                ":type  socket: UDTSOCKET               \n"
                "\n"
                ":return: host, port \n"
                ":type:   str(),int()\n"
                
        },
        {
                "setsockopt",
                (PyCFunction)pyudt4_setsockopt,
                METH_VARARGS,
                "Set a socket option value.                     \n"
                "\n"
                ":param socket: Socket to act on                \n"
                ":type  socket: UDTSOCKET                       \n"
                "\n"
                ":param option: enumerated option to be setting \n"
                ":type  option: int()                           \n"
                "\n"
                ":param optval: Value to set option to          \n"
                ":type  optval: int(), long(), bool(), tuple()  \n"
        },
        {
                "getsockopt",
                (PyCFunction)pyudt4_getsockopt,
                METH_VARARGS,
                "Get a socket option value.                     \n"
                "\n"
                ":param socket: Socket to act on                \n"
                ":type  socket: UDTSOCKET                       \n"
                "\n"
                ":param option: enumerated option to be getting \n"
                ":type  option: int()                           \n"
                "\n"
                ":return:       Value associated with option    \n"
                ":type  :       int(), long(), bool(), tuple()  \n" 
        },
        {
                "getsockstate",
                (PyCFunction)pyudt4_getsockstate,
                METH_VARARGS,
                "Return the current socket state"
                "\n"
                ":param socket: Socket to act on                \n"
                ":type  socket: UDTSOCKET                       \n"
        },
        {
                "send",
                (PyCFunction)pyudt4_send,
                METH_VARARGS,
                ""
                "\n"
                ":param socket: Socket to act on                \n"
                ":type  socket: UDTSOCKET                       \n"
                "\n"
                ":param buf:    Write buffer                    \n"
                ":type  buf:    str()                           \n"
                "\n"
                ":param len:    Length of the incoming buffer   \n"
                ":type  len:    int()                           \n"
                "\n"
                ":param pad:    Total send size (fix message on to pad of \n" 
                "               this size)\n"
                ":type  pad:    int()" 
        },
        {
                "recv",
                (PyCFunction)pyudt4_recv,
                METH_VARARGS,
                ""
                "\n"
                ":param socket: Socket to act on                \n"
                ":type  socket: UDTSOCKET                       \n"
                "\n"
                ":param len   : Number of bytes to read         \n"
                ":type  len   : int()                           \n"
                "\n"
                ":return: Read-in data \n"
                ":type  : str()" 
        },
        {
                "sendmsg",
                (PyCFunction)pyudt4_sendmsg,
                METH_VARARGS,
                 "\n"
                ":param socket:   Socket to act on                \n"
                ":type  socket:   UDTSOCKET                       \n"
                "\n"
                ":param buf:      Write buffer                    \n"
                ":type  buf:      str()                           \n"
                "\n"
                ":param len:      Length of the incoming buffer   \n"
                ":type  len:      int()                           \n"
                "\n"
                ":param pad:      Total send size (fix message on to pad of \n" 
                "                 this size)\n"
                ":type  pad:      int()     \n"
                "\n"
                ":param ttl:      Time-to-live of message on MS   \n"
                ":type  ttl:      int()                           \n"
                "\n"
                ":param inorder:  Send the data in order?        \n"
                ":type  inorder:  bool()                         \n"
        },
        {
                "recvmsg",
                (PyCFunction)pyudt4_recvmsg,
                METH_VARARGS,
                ""
                ":param socket:   Socket to act on                \n"
                ":type  socket:   UDTSOCKET                       \n"
                "\n"
                ":param len:      Number of bytes to read in      \n"
                ":type  len:      int()                           \n"
                "\n"
                ":return:       The received message \n"
                ":type  :       str()                \n"

        },
        {
                "sendfile",
                (PyCFunction)pyudt4_sendfile,
                METH_VARARGS,
                "Only semi-implemented: DO NOT USE"
        },
        {
                "recvfile",
                (PyCFunction)pyudt4_recvfile,
                METH_VARARGS,
                "Only semi-implemented: DO NOT USE"
        },
        {
                "perfmon",
                (PyCFunction)pyudt4_perfmon,
                METH_VARARGS,
                "Return the TRACEINFO structure associated with the socket \n"
                "\n"
                ":param socket:   Socket to act on                \n"
                ":type  socket:   UDTSOCKET                       \n"
                "\n"
                ":return:       Trace info for socket   \n"
                ":type  :       TRACEINFO               \n" 
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
                                        (char*) "udt4.UDTException", 
                                        PyExc_RuntimeError,     0x0
                                        );
        Py_INCREF(pyudt4_exception_obj); 
        PyModule_AddObject(
                        m, "UDTException", pyudt4_exception_obj
                        );
        
        
        if (PyType_Ready(&pyudt4_perfmon_type) < 0)
                return;

        pyudt4_perfmon_type.tp_new = PyType_GenericNew;  
        Py_INCREF(&pyudt4_perfmon_type);
        PyModule_AddObject(
                m, "TRACEINFO", (PyObject*) &pyudt4_perfmon_type
                );

        
        pyudt4_socket_type   = initpyudt4_socket_type(m);
        pyudt4_epoll_type    = initpyudt4_epoll_type(m, pyudt4_exception_obj, 
                                                     pyudt4_socket_type);
        
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
           ) 
                return;
        
        /* EPOLLOpt */
        if (
            PyModule_AddIntConstant(m, "UDT_EPOLL_IN"  , UDT_EPOLL_IN  ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_EPOLL_OUT" , UDT_EPOLL_OUT ) < 0 ||
            PyModule_AddIntConstant(m, "UDT_EPOLL_ERR" , UDT_EPOLL_ERR ) < 0 
           ) 
                return;


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
            ) 
                return;
       

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
           ) 
                return ;
}

#ifdef __cplusplus
}
#endif 
