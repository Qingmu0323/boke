#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <assert.h>
#include <netdb.h>  /* netdb is necessary for struct hostent */
#include "../include/header.h"
#include "../include/thread_pool.h"

#define PORT 4321  /* server port */

#define MAXDATASIZE 100
static int sum = 0;

void *client_sock_recv_proc(void * arg)
{
    pthread_detach(pthread_self()); 
    printf("asdasdasdasd\n");
}

void * send_sock(void *arg)
{
    int sockfd = (int)arg;
    pthread_detach(pthread_self()); 
    printf("send once\n");
    sum++;
    char package[100] = "test";
    int num;
    if((num=send(sockfd,package,sizeof(SEND_PACKAGE),0))==-1){
        printf("send() error\n");
        exit(1);
    }
    if((num=recv(sockfd,package,MAXDATASIZE,0))==-1)
    {
        printf("recv() error\n");
        exit(1);
    }

    printf("server message: %s\n",package);
}

void start_thread_pool(void *sockfd)
{
    printf("thread_pool start work \n");
    thpool_t * thpool;
    thpool = thpool_init(5);
    int i;
    for (i = 0; i < 5; ++i)
    {
        printf("for \n");
        thpool_add_work(thpool,(send_sock),sockfd);
    }
    
}

int main(int argc, char *argv[])
{
    int sockfd, num;    /* files descriptors */
    char buf[MAXDATASIZE];    /* buf will store received text */
    struct hostent *he;    /* structure that will get information about remote host */
    struct sockaddr_in server;
    SEND_PACKAGE *package,*package_rec;
    SEND_HEAD *head;
    
    
    if((he=gethostbyname("127.0.0.1"))==NULL)
    {
        printf("gethostbyname() error\n");
        exit(1);
    }
    
    if((sockfd=socket(AF_INET,SOCK_STREAM, 0))==-1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server))==-1)
    {
        printf("connect() error\n");
        exit(1);
    }
    package = malloc(sizeof(SEND_PACKAGE));
    package_rec = malloc(sizeof(SEND_PACKAGE));
    head = malloc(sizeof(SEND_HEAD));
    unsigned char a = 'a';

    // memcpy(head,a,1);
    
    package->head = a;
    
    char str[] = "horst\n";
    memcpy(package->body,str,sizeof(str));
    package->length = sizeof(str);

    /**
    test send file
    */
    // int filefd = open( "test.txt", O_RDONLY );
    // assert( filefd > 0 );
    // struct stat stat_buf;
    // fstat( filefd, &stat_buf );

    
    // sendfile(sockfd,filefd,NULL,stat_buf.st_size);

    start_thread_pool((void *)sockfd);

    // pthread_t tid = 0;
    // int i,ret;
    // ret = pthread_create(&tid, NULL, client_sock_recv_proc,(void*)sockfd);
    // if ( ret == 0 )
    //     {
    //         printf("create thread success sock:%d\n", ret);
    //     }
    //     else
    //     {
    //         printf("create thread failed %d\n", ret);
    //     }    
    // for (i = 0; i < 5; ++i)
    // {
    //     printf("sum is %d\n",sum);
    //     ret = pthread_create(&tid, NULL, send_sock,(void*)sockfd);
    //     if ( ret == 0 )
    //     {
    //         printf("create thread success sock:%d\n", ret);
    //     }
    //     else
    //     {
    //         printf("create thread failed %d\n", ret);
    //     }        
    // }
    
    
    // if((num=send(sockfd,package,sizeof(SEND_PACKAGE),0))==-1){
    //     printf("send() error\n");
    //     exit(1);
    // }
    // if((num=recv(sockfd,package_rec,MAXDATASIZE,0))==-1)
    // {
    //     printf("recv() error\n");
    //     exit(1);
    // }

    // printf("server message: %s\n",package_rec->body);
    // close(sockfd);
    while(1){}
    return 0;
}

