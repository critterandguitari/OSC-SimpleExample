/* Copyright (c) 2000 Miller Puckette.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in the Pd distribution.  */

/* the "pdreceive" command. This is a standalone program that receives messages
from Pd via the netsend/netreceive ("FUDI") protocol, and copies them to
standard output. */

/* May 2008 : fixed a buffer overflow problem; pdreceive sometimes 
    repeated infinitely its buffer during high speed transfer. 
    Moonix::Antoine Rousseau
*/

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
#include <fcntl.h>
#include "pd_receive.h"
#define SOCKET_ERROR -1

typedef struct _fdpoll
{
    int fdp_fd;
    char *fdp_outbuf;/*output message buffer*/ 
    int fdp_outlen;     /*length of output message*/
    int fdp_discard;/*buffer overflow: output message is incomplete, discard it*/
    int fdp_gotsemi;/*last char from input was a semicolon*/
} t_fdpoll;

static int nfdpoll;
static t_fdpoll *fdpoll;
static int maxfd;
static int sockfd;
static int protocol;

static void sockerror(char *s);
static void x_closesocket(int fd);

int pd_receive_init(void)
{
    int portno;
    struct sockaddr_in server;
    int nretry = 10;
       
    portno = PD_RECV_PORT;
 
    protocol = SOCK_DGRAM;
    
    sockfd = socket(AF_INET, protocol, 0);
    if (sockfd < 0)
    {
        sockerror("socket()");
        exit(1);
    }

    // set to non blocking
    int x;
    x = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, x | O_NONBLOCK);

    maxfd = sockfd + 1;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

        /* assign client port number */
    server.sin_port = htons((unsigned short)portno);

        /* name the socket */
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        sockerror("bind");
        x_closesocket(sockfd);
        return (0);
    }
}

int udpread(char *buf)
{
      
    //int ret = recvfrom(sockfd, buf, 10000, 0, NULL, NULL);
    int ret = recv(sockfd, buf, PD_RECV_BUFSIZE, MSG_DONTWAIT);  // with non blocking
    if (ret < 0)
    {
        sockerror("recv (udp)");
        x_closesocket(sockfd);
        return -1;
    }
    return ret;
}

int pd_receive_poll(char *buf)
{
    int i;
    t_fdpoll *fp;
    fd_set readset, writeset, exceptset;
    FD_ZERO(&writeset);
    FD_ZERO(&readset);
    FD_ZERO(&exceptset);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_SET(sockfd, &readset);
    //printf("gonna select the socket now \n");
    if (select(maxfd+1, &readset, &writeset, &exceptset, &tv) < 0)
    {
        perror("select");
        return -1;
    }
    if (FD_ISSET(sockfd, &readset))
        return udpread(buf);
    else return 0;
}

static void sockerror(char *s)
{
    int err = errno;
    fprintf(stderr, "%s: %s (%d)\n", s, strerror(err), err);
}

static void x_closesocket(int fd)
{
    close(fd);
}
