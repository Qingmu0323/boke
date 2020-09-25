#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "header.h"
#include <linux/tcp.h>
#include <pthread.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>

#include "server.h"
#include "thread_pool.h"
#include "reactor.h"


static thpool_t * thpool;
static int pipefd[2];


int main(int argc, char *argv[])
{
    int ret,listenfd,epollfd;
    
    printf("father process\n");
    thpool = thpool_init(7);
    ret = log_init();
    if (ret)
    {
        printf("log_init failed\n");
        exit(0);
    }

    ret = init_server(&listenfd,&epollfd);
    if (ret)
    {
        printf("init_server failed\n");
    }

    printf("#test_use_reactor start #\n");
    ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
    assert( ret != -1 );
    //基于Reactor构架进行对象编程
    reactor_t *reactor = create_reactor(epollfd,thpool);
    //对监听句柄封装event_handler 
    
    if (acceptor == NULL)
    {
        printf("acceptor is NULL\n");
    }
    reactor->add_eh(reactor,create_acceptor(listenfd));

    event_handler_t *sig_handler = create_signal_accepter(reactor,SIGALRM);
    reactor->add_eh(reactor,sig_handler);

    alarm( TIMESLOT );

    reactor->event_loop(reactor);
    return 0;
}


int init_server(int *listenfd,int *epollfd)
{
    int ret = 0;
    struct sockaddr_in server;
    *listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (*listenfd == -1)
    {
        printf("create socket err\n");
        return -1;
    }

    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(*listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(*listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        return -1;
    }
    
    if(listen(*listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        return -1;
    }

    *epollfd = epoll_create(5);
    if (*epollfd == -1)
    {
        printf("init_server epoll create err\n");
        return -1;
    }
    return ret;
}

