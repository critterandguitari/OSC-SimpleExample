

/* Copyright (c) 2000 Miller Puckette.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in the Pd distribution.  */

/* the "pdsend" command.  This is a standalone program that forwards messages
from its standard input to Pd via the netsend/netreceive ("FUDI") protocol. */

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "udp_send.h"
#define SOCKET_ERROR -1

void sockerror(char *s);
void x_closesocket(int fd);

int sockfd;


// send the line to pd
int udp_send_buf(uint8_t * buf, int len) {
   
    int res = send(sockfd, buf, len, 0);
    //printf("sent %s %d %d...\n", line, nsend, res);
    if (res < 0)
    {
//        sockerror("send");
    }
}

int udp_send_init(void)
{
    int portno, protocol;
    struct sockaddr_in server;
    struct hostent *hp;
    int nretry = 10;
    
    const char *hostname = "127.0.0.1";
//    hostname = "192.168.1.121";
    portno = UDP_SEND_PORT;
    
    protocol = SOCK_DGRAM;

    sockfd = socket(AF_INET, protocol, 0);
    if (sockfd < 0)
    {
        sockerror("socket()");
        return -1;
    }
        /* connect socket using hostname provided in command line */
    server.sin_family = AF_INET;
    hp = gethostbyname(hostname);
    if (hp == 0)
    {
        fprintf(stderr, "%s: unknown host\n", hostname);
        x_closesocket(sockfd);
        return -1;
    }
    memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);

        /* assign client port number */
    server.sin_port = htons((unsigned short)portno);

    /* try to connect.  */
    if (connect(sockfd, (struct sockaddr *) &server, sizeof (server)) < 0)
    {
        sockerror("connect");
        x_closesocket(sockfd);
        return -1;
    }
}



void sockerror(char *s)
{
    int err = errno;
    fprintf(stderr, "%s: %s (%d)\n", s, strerror(err), err);
}

void x_closesocket(int fd)
{
    close(fd);
}
