
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arpa/inet.h>

#include <udt>


static int _G_port = 3001;

int 
main(int argc, char *argv[]) 
{
        /* load server */
        UDTSOCKET sock = UDT::socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in serv;
        serv.sin_family = AF_INET;
        serv.sin_port   = htons(_G_port);
        inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);
        
        memset(serv.sin_zero, '\0', sizeof(serv.sin_zero));
        
        assert(
                UDT::ERROR != UDT::connect(sock, (sockaddr*) &serv, sizeof(serv))
              );


        /* BEGIN test */
        int msg_len;
        char buffer[1024];

        /* recv message */
        memset(buffer, '\0', sizeof(buffer));
        
        assert(
                sizeof(int) == UDT::recv(sock, (char*) &msg_len, sizeof(int),
                                         0)
              );
        assert(
                msg_len     == UDT::recv(sock, buffer, msg_len, 0)
              );
        assert(strlen(buffer) == msg_len);

        fprintf(stderr, "received message: %s\n", buffer);

        /* send message */
        memset(buffer, '\0', sizeof(buffer));
        
        strcpy(buffer, "no bottle found");
        msg_len = strlen(buffer);

        assert(
                sizeof(int) == UDT::send(sock, (char*) &msg_len, sizeof(int),
                                         0)
              );
        assert(
                msg_len     == UDT::send(sock, buffer, msg_len, 0)
              );

        /* cleanup */
        UDT::close(sock);
}
