

import socket   as     socklib  
import udt4     as     udt 
from   unittest import TestCase
from   threading import Thread 

OPTIONS = [ 

            ]


class SockTest(TestCase):
    def setUp(self): 
        self.sender = udt.socket(socklib.AF_INET, socklib.SOCK_STREAM 
                                , socklib.AI_PASSIVE)
        self.recevr = udt.socket(socklib.AF_INET, socklib.SOCK_STREAM 
                                , socklib.AI_PASSIVE)
        

    def runServer(self):
        udt.bind  (self.sender, '127.0.0.1', 8001)
        udt.listen(self.sender, 10) 

        client, host = udt.accept(self.sender)

        for k, i in enumerate([('a', 1), ('b', 2), ('c', 3)]):
            buf = i[0] * (10 ** i[1]) 

            if k % 2 == 0:
                self.assertEquals(
                        udt.send(client, buf, len(buf)), len(buf)
                        )
            else:
                self.assertEquals(
                        udt.recv(client, len(buf)), buf
                        )


    def runTest(self):
        Thread(target = self.runServer).start()
        
        udt.connect(self.recevr, '127.0.0.1', 8001)
        
        for k, i in enumerate([('a', 1), ('b', 2), ('c', 3)]):
            buf = i[0] * (10 ** i[1]) 

            if k % 2 != 0:
                self.assertEquals(
                        udt.send(self.recevr, buf, len(buf)), len(buf)
                        )
            else:
                self.assertEquals(
                        udt.recv(self.recevr, len(buf)), buf
                        )


class DgramTest(TestCase):
    def setUp(self):
        self.sender = udt.socket(socklib.AF_INET, socklib.SOCK_DGRAM
                                , socklib.AI_PASSIVE)
        self.recevr = udt.socket(socklib.AF_INET, socklib.SOCK_DGRAM
                                , socklib.AI_PASSIVE)
        

    def runServer(self):
        udt.bind  (self.sender, '127.0.0.1', 8002)
        udt.listen(self.sender, 10) 

        client, host = udt.accept(self.sender)

        for k, i in enumerate([('a', 1), ('b', 2), ('c', 3)]):
            buf = i[0] * (10 ** i[1]) 

            if k % 2 == 0:
                self.assertEquals(
                        udt.sendmsg(client, buf, len(buf)), len(buf)
                        )
            else:
                self.assertEquals(
                        udt.recvmsg(client, len(buf)), buf
                        )


    def runTest(self):
        Thread(target = self.runServer).start()
        
        udt.connect(self.recevr, '127.0.0.1', 8002)
        
        for k, i in enumerate([('a', 1), ('b', 2), ('c', 3)]):
            buf = i[0] * (10 ** i[1]) 

            if k % 2 != 0:
                self.assertEquals(
                        udt.sendmsg(self.recevr, buf, len(buf)), len(buf)
                        )
            else:
                self.assertEquals(
                        udt.recvmsg(self.recevr, len(buf)), buf
                        )
