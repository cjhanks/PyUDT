
"""
:module: __init__.py 

--------------------------------------------------------------------------------
Namespace: udt4
--------------------------------------------------------------------------------
udt4
    |
    +-- .UDTSOCKET          : Raw UDTSOCKET reference 
    +-- .UDTepoll           : epoll implementation accepting UDTSOCKET instances
    +-- .UDTException       : Maps to UDTEXCEPTION (error_code, error_string) 
    +-- .TRACEINFO          : Direct structure map to UDT::TRACEINFO 
    |
    .pyudt
         |
         +-- .UdtSocket()   : Python style socket wrapper around UDTSOCKET  
         |
         +-- .Epoll()       : Epoll implementation to be used with UdtSocket()

--------------------------------------------------------------------------------
Versioning:
Releases are done in {MAJOR}.{MINOR} of py-udt version.  All function 
signatures are back-compatible on major and minor releases.

If and when UDT releases udt5, the py-udt library will be ported to v5, and 
the py-udt4 will remain supported independently. 

--------------------------------------------------------------------------------
License:
UDT4 library and py-udt4 wrapper is licensed under BSD.

--------------------------------------------------------------------------------
"""

__author__  = 'Christopher J. Hanks <develop@cjhanks.name>'
__date__    = '12/18/2012' 
__license__ = 'GPLv3' 


import os 

import _udt4
from   _udt4 import *
import pyudt

__all__ = os._get_exports_list(_udt4) + \
         ['UdtSocket', 'dump_perfmon', 'Epoll'] 


def dump_perfmon(perf):
    """
    Prints out all of the perfmon features
    """
    print(
'''
+ TRACEINFO ----------------- +
| msTimeStamp         %Li 
| pktSentTotal        %Li
| pktRecvTotal        %Li 
| pktSndLossTotal     %i 
| pktRcvLossTotal     %i 
| pktRetransTotal     %i 
| pktSentACKTotal     %i 
| pktRecvACKTotal     %i 
| pktSentNAKTotal     %i 
| pktRecvNAKTotal     %i 
| --                    -
| pktSent             %Li
| pktRecv             %Li
| pktSndLoss          %i 
| pktRcvLoss          %i 
| pktRetrans          %i 
| pktSentACK          %i 
| pktRecvACK          %i 
| pktSentNAK          %i 
| pktRecvNAK          %i 
| mbpsSendRate        %f 
| mbpsRecvRate        %f 
| --                    -
| usPktSndPeriod      %f 
| pktFlowWindow       %i 
| pktCongestionWindow %i 
| pktFlightSize       %i 
| msRTT               %f 
| mbpsBandwidth       %f 
| byteAvailSndBuf     %i 
| byteAvailRcvBuf     %i 
+---------------------------- +
''' % (perf.msTimeStamp    , perf.pktSentTotal   , perf.pktRecvTotal, 
       perf.pktSndLossTotal, perf.pktRcvLossTotal, perf.pktRetransTotal,
       perf.pktSentACKTotal, perf.pktRecvACKTotal, perf.pktSentNAKTotal,
       perf.pktRecvNAKTotal, 

       perf.pktSent        , perf.pktRecv        , perf.pktSndLoss,
       perf.pktRcvLoss     , perf.pktRetrans     , perf.pktSentACK, 
       perf.pktRecvACK     , perf.pktSentNAK     , perf.pktRecvNAK, 
       perf.mbpsSendRate   , perf.mbpsRecvRate   ,

       perf.usPktSndPeriod , perf.pktFlowWindow  , perf.pktCongestionWindow,
       perf.pktFlightSize  , perf.msRTT          , perf.mbpsBandwidth,
       perf.byteAvailSndBuf, perf.byteAvailRcvBuf 
       )
    )
