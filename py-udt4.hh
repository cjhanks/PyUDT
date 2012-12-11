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
