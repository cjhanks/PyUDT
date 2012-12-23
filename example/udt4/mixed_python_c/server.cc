
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
        serv.sin_addr.s_addr = INADDR_ANY;

        memset(serv.sin_zero, '\0', sizeof(serv.sin_zero));

        if (UDT::ERROR == UDT::bind(sock, (sockaddr*) &serv, sizeof(serv)))
                return 1;
        else
                UDT::listen(sock, 10);

        
        /* BEGIN test */
        UDTSOCKET client = UDT::accept(sock, 0x0, 0);
        
        int  msg_len;
        char buffer[1024];
        
        /* send message */
        memset(buffer, '\0', sizeof(buffer));
        
        strcpy(buffer, "message in a bottle");
        msg_len = strlen(buffer);
        
        assert(
                sizeof(int) == UDT::send(client, (char*) &msg_len, sizeof(int),
                                        0)
              );
        assert(
                msg_len     == UDT::send(client, buffer, msg_len, 0)
              );

        /* recv message */ 
        memset(buffer, '\0', sizeof(buffer));
        
        assert(
                sizeof(int) == UDT::recv(client, (char*) &msg_len, sizeof(int), 
                                         0)
              );

        assert(
                msg_len     == UDT::recv(client, buffer, msg_len, 0)
              );

        fprintf(stderr,
                "received message: %s\n", buffer
               );

        assert(strlen(buffer) == msg_len);
        
        /* clean up */
        UDT::close(client);
        UDT::close(sock);

        return 0;
}
