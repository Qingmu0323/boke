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

#define PORT 4321  /* server port */
#define FILE_PORT 4322

#define MAXDATASIZE 4112
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

    int fp;
    struct stat stat_buf;
    fp = open("file.txt",O_RDONLY);
    if (fp == -1)
    {
        printf("read err\n");
    }
    char buf[1000];
    int r = read(fp,buf,1000);
    buf[999] = '\0';
    printf("buf is %s\n r is %d", buf,r);
    fstat(fp, &stat_buf);
    off_t offset = 0;
    // int res = sendfile(sockfd,fp,&offset,stat_buf.st_size);
    // if (res < 0)
    // {
    //     printf("sendfile error\n");
    // }
    

    common_package_t *common_package = malloc(sizeof(common_package_t));
    char user_name[20] = "admin";
    char user_pwd[20] = "123456";
    memcpy(common_package->body,user_name,20);
    common_package->body[19] = '\0';
    printf("user_name %s",user_name  );
    printf("common_package->body %s\n",common_package->body);
    memcpy(common_package->body+20,user_pwd,20);
    common_package->body[39] = '\0';
    printf("common_package->body %s\n",common_package->body+20);

    common_package->src_id = 20000;
    common_package->des_id = 5;
    common_package->msg_type = 4;

    // memcpy(common_package->body,buf,1000);

    int num;
    if((num=send(sockfd,common_package,sizeof(common_package_t),0))==-1){
        printf("send() error\n");
        exit(1);
    }
    if((num=recv(sockfd,common_package,sizeof(common_package_t),0))==-1)
    {
        printf("recv() error\n");
        exit(1);
    }

    printf("server message: %s num is %d \n",common_package->body,num); 
    // close(sockfd);
    // while(1)
    // {
    //    if((num=recv(sockfd,common_package,MAXDATASIZE,0))==-1)
    //     {
    //         printf("recv() error\n");
    //         exit(1);
    //     }

    //     printf("server message: %s num is %d \n",common_package->body,num); 
    // }
    
}


int main(int argc, char *argv[])
{
    int sockfd, num;    /* files descriptors */
    char buf[MAXDATASIZE];    /* buf will store received text */
    struct hostent *he;    /* structure that will get information about remote host */
    struct sockaddr_in server;
    SEND_PACKAGE *package,*package_rec;
    SEND_HEAD *head;
    
    if (argc != 2)
    {
        printf("Usage: %s <IP Address>\n",argv[0]);
        exit(1);
    }
    
    if((he=gethostbyname(argv[1]))==NULL)
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
    pthread_t tid = 0;
    int i,ret;    
    for (i = 0; i < 5; ++i)
    {
        printf("sum is %d\n",sum);
        ret = pthread_create(&tid, NULL, send_sock,(void*)sockfd);
        if ( ret == 0 )
        {
            printf("create thread success sock:%d\n", ret);
        }
        else
        {
            printf("create thread failed %d\n", ret);
        }        
    }
    
    
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

