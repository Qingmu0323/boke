#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>     //使用信号头文件
#include <unistd.h>     //系统调用API
#include <arpa/inet.h>  //网络编程对应的头文件(htonl htons ntohs inet_addr inet_ntoa等)
#include <sys/types.h>  //基本系统数据类型(size_t，time_t，pid_t等)
#include <sys/socket.h> 
#include <netinet/in.h> //struct socketaddr_in   in_addr_t s_addr 等
#include <netinet/ip.h> //struct ip
#include <netinet/ip_icmp.h> //struct icmphdr
#include <netdb.h>      //与网络有度关问的结构 (gethostbyname             gethostbyaddr)
#include <setjmp.h>
#include <error.h>      //错误宏的集合


#define PACKET_SIZE     	4096  
#define MAX_WAIT_TIME   	5
#define MAX_NO_PACKETS  	4
#define DATA_LEN			56  //整个icmp头的长度

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];

int nsend=0,nreceived=0;
struct timeval tvrecv;

struct sockaddr_in from;

/*
//打印收发包的序号并计算丢包率
void statistics(int signo);
//crc32的校验计算方法
unsigned short cal_chksum(unsigned short *addr,int len);
//组包
int pack(int pack_no,int pid);
//发包
void send_packet(int sockfd, int pid,struct sockaddr_in dest_addr);
//收包
void recv_packet(int sockfd,int pid);
//解包
int unpack(char *buf,int len,int pid);
//计算ping包从发送到接收所用的时间
void tv_sub(struct timeval *out,struct timeval *in);
*/

//crc32的校验计算方法
unsigned short cal_chksum(unsigned short *addr,int len)
{       
	int nleft=len;        
	int sum=0;        
	unsigned short *w=addr;        
	unsigned short answer=0;	    
	
	while(nleft>1)        
	{       
		sum+=*w++;                
		nleft-=2;        
	}
	
	if( nleft==1)        
	{       
		*(unsigned char *)(&answer)=*(unsigned char *)w;                
		sum+=answer;        
	}        

	sum=(sum>>16)+(sum&0xffff);        
	sum+=(sum>>16);        
	answer=~sum;        

	return answer;
}
//打印收发包的序号并计算丢包率
void statistics(int signo)
{
	printf("\n--------------------PING statistics-------------------\n");    
	printf("%d packets transmitted, %d received , %%%f lost\n",nsend,nreceived,(nsend-nreceived)*1.0/nsend*100);        
	       
	exit(1);
}


//组包   协议填充
int pack(int pack_num, int pid)
{
    int i,packsize;
    struct icmp *icmp;
    struct timeval *tval;

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;   //类型
    icmp->icmp_code = 0;           //代码
    icmp->icmp_cksum = 0;          //校验和
    icmp->icmp_seq = pack_num;     //序号
    icmp->icmp_id = pid;           //id
    packsize = 8 +DATA_LEN;        //整个包的长度          8是timeval的长度        
    tval = (struct timeval *)icmp->icmp_data;

    gettimeofday(tval,NULL);//获取时间放入tval中

    icmp->icmp_cksum = cal_chksum((unsigned short *)icmp,packsize);
    return packsize;
}

//发包  
void send_packet(int sockfd, int pid, struct sockaddr_in dest_addr)
{
    //包的大小
    int packetsize = 0;

    //发送MAX_NO_PACKETS个包
    while ( nsend < MAX_NO_PACKETS)
    {
        nsend++;
        packetsize = pack(nsend,pid);
        if (sendto(sockfd,sendpacket,packetsize,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr)) <0 )
        {
            perror("sendto error\n");
            continue;
        }
        sleep(1);
    }

}
//解包
int unpack(char * buf, int len, int pid)
{
    int i,iphdrlen;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;
    double rtt;

    ip=(struct ip *)buf;
    iphdrlen = ip->ip_hl<<2; //恢复ip头的长度

    icmp = (struct icmp *)(buf + iphdrlen);

    len -=iphdrlen;
    if(len < 8)
    {
        printf("ICMP packets\'s length is less than 8\n");
        return -1;
    }

    //判断是否是ping的回复与是否是当前进程
    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))
    {
        tvsend = (struct timeval *)icmp->icmp_data;
        //获取ping包从发送到接收所用的时间
        tv_sub(&tvrecv,tvsend);
        rtt=tvrecv.tv_sec*1000+tvrecv.tv_usec/1000;                
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",len,inet_ntoa(from.sin_addr),icmp->icmp_seq, ip->ip_ttl,rtt);   
        
    }


}
//收包
void recv_packet(int sockfd, int pid)
{
    int n,fromlen;
    //extern int errno;
    signal(SIGALRM,statistics);       
    fromlen = sizeof(from);
    while ( nreceived < nsend )
    {
        //设置定时器 函数会在所指定的seconds之后收到SIGALRM信号
        alarm(MAX_WAIT_TIME);
        /*if(n = recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,&fromlen) <0)
        {
            //调用系统调用被中断，返回错误码EINTR
            if(errno == EINTR)
                continue;
            perror("recvfrom error\n");
            continue;
        }*/
        n=recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,&fromlen);
        gettimeofday(&tvrecv,NULL);
        if(unpack(recvpacket,n,pid) == -1)
            continue;
        nreceived++;
    }

}

//计算ping包从发送到接收所用的时间
void tv_sub(struct timeval *out,struct timeval *in)
{       
	if( (out->tv_usec-=in->tv_usec)<0)        
	{       
		--out->tv_sec;                
		out->tv_usec+=1000000;        
	}        
	out->tv_sec-=in->tv_sec;
}


int main(int argc , char **argv)
{
    int sockfd;
    //目的sockaddr_in对象
    struct sockaddr_in dest_addr;
    pid_t pid;
    //in_addr_t inaddr=0;
    unsigned long inaddr=0;

    struct hostent *host;
    //等待时间
    int waittime = MAX_WAIT_TIME;
    
    int size = 50 * 1024;

    if(argc < 2)
    {
        printf("usage : %s,hostname /IP address\n",argv[0]);
        exit(1);
    }

    //创建ICMP套接字
    if((sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)
    {
        perror("Create sockfd error\n");
        exit(1);
    }

    //提升权限位root权限
    setuid(getuid());

    //改变接受buf缓冲区的大小
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));

    //清空sockaddr_in dest_addr
    bzero(&dest_addr,sizeof(dest_addr));

    //设置dest_addr
    dest_addr.sin_family = AF_INET;
    
    //进行DNS域名解析
    if((inaddr = inet_addr(argv[1])) == INADDR_NONE)  //判断是否是无效的IP地址
    {
        //进行域名解析
        if((host = gethostbyname(argv[1])) == NULL)
        {
            perror("gethostbyname error\n");
            exit(1);
        }
        memcpy((char *)&(dest_addr.sin_addr),host->h_addr,host->h_length);
    }
    else
    {
        memcpy((char *)&(dest_addr.sin_addr),(char *)&inaddr,sizeof(inaddr));
    }
    
    printf("PING %s(%s) : %d byte data in ICMP packets. \n",argv[1],inet_ntoa(dest_addr.sin_addr),DATA_LEN);

    //获取当前进程pid
    pid = getpid();

    //发包
    send_packet(sockfd,pid,dest_addr);

    //收包
    recv_packet(sockfd,pid);

    statistics(SIGALRM);

    //关闭套接字
    close(sockfd);

    return 0;
}
