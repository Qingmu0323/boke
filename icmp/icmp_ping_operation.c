#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>     //ʹ���ź�ͷ�ļ�
#include <unistd.h>     //ϵͳ����API
#include <arpa/inet.h>  //�����̶�Ӧ��ͷ�ļ�(htonl htons ntohs inet_addr inet_ntoa��)
#include <sys/types.h>  //����ϵͳ��������(size_t��time_t��pid_t��)
#include <sys/socket.h> 
#include <netinet/in.h> //struct socketaddr_in   in_addr_t s_addr ��
#include <netinet/ip.h> //struct ip
#include <netinet/ip_icmp.h> //struct icmphdr
#include <netdb.h>      //�������жȹ��ʵĽṹ (gethostbyname             gethostbyaddr)
#include <setjmp.h>
#include <error.h>      //�����ļ���


#define PACKET_SIZE     	4096  
#define MAX_WAIT_TIME   	5
#define MAX_NO_PACKETS  	4
#define DATA_LEN			56  //����icmpͷ�ĳ���

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];

int nsend=0,nreceived=0;
struct timeval tvrecv;

struct sockaddr_in from;

/*
//��ӡ�շ�������Ų����㶪����
void statistics(int signo);
//crc32��У����㷽��
unsigned short cal_chksum(unsigned short *addr,int len);
//���
int pack(int pack_no,int pid);
//����
void send_packet(int sockfd, int pid,struct sockaddr_in dest_addr);
//�հ�
void recv_packet(int sockfd,int pid);
//���
int unpack(char *buf,int len,int pid);
//����ping���ӷ��͵��������õ�ʱ��
void tv_sub(struct timeval *out,struct timeval *in);
*/

//crc32��У����㷽��
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
//��ӡ�շ�������Ų����㶪����
void statistics(int signo)
{
	printf("\n--------------------PING statistics-------------------\n");    
	printf("%d packets transmitted, %d received , %%%f lost\n",nsend,nreceived,(nsend-nreceived)*1.0/nsend*100);        
	       
	exit(1);
}


//���   Э�����
int pack(int pack_num, int pid)
{
    int i,packsize;
    struct icmp *icmp;
    struct timeval *tval;

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;   //����
    icmp->icmp_code = 0;           //����
    icmp->icmp_cksum = 0;          //У���
    icmp->icmp_seq = pack_num;     //���
    icmp->icmp_id = pid;           //id
    packsize = 8 +DATA_LEN;        //�������ĳ���          8��timeval�ĳ���        
    tval = (struct timeval *)icmp->icmp_data;

    gettimeofday(tval,NULL);//��ȡʱ�����tval��

    icmp->icmp_cksum = cal_chksum((unsigned short *)icmp,packsize);
    return packsize;
}

//����  
void send_packet(int sockfd, int pid, struct sockaddr_in dest_addr)
{
    //���Ĵ�С
    int packetsize = 0;

    //����MAX_NO_PACKETS����
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
//���
int unpack(char * buf, int len, int pid)
{
    int i,iphdrlen;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;
    double rtt;

    ip=(struct ip *)buf;
    iphdrlen = ip->ip_hl<<2; //�ָ�ipͷ�ĳ���

    icmp = (struct icmp *)(buf + iphdrlen);

    len -=iphdrlen;
    if(len < 8)
    {
        printf("ICMP packets\'s length is less than 8\n");
        return -1;
    }

    //�ж��Ƿ���ping�Ļظ����Ƿ��ǵ�ǰ����
    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))
    {
        tvsend = (struct timeval *)icmp->icmp_data;
        //��ȡping���ӷ��͵��������õ�ʱ��
        tv_sub(&tvrecv,tvsend);
        rtt=tvrecv.tv_sec*1000+tvrecv.tv_usec/1000;                
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",len,inet_ntoa(from.sin_addr),icmp->icmp_seq, ip->ip_ttl,rtt);   
        
    }


}
//�հ�
void recv_packet(int sockfd, int pid)
{
    int n,fromlen;
    //extern int errno;
    signal(SIGALRM,statistics);       
    fromlen = sizeof(from);
    while ( nreceived < nsend )
    {
        //���ö�ʱ�� ����������ָ����seconds֮���յ�SIGALRM�ź�
        alarm(MAX_WAIT_TIME);
        /*if(n = recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,&fromlen) <0)
        {
            //����ϵͳ���ñ��жϣ����ش�����EINTR
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

//����ping���ӷ��͵��������õ�ʱ��
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
    //Ŀ��sockaddr_in����
    struct sockaddr_in dest_addr;
    pid_t pid;
    //in_addr_t inaddr=0;
    unsigned long inaddr=0;

    struct hostent *host;
    //�ȴ�ʱ��
    int waittime = MAX_WAIT_TIME;
    
    int size = 50 * 1024;

    if(argc < 2)
    {
        printf("usage : %s,hostname /IP address\n",argv[0]);
        exit(1);
    }

    //����ICMP�׽���
    if((sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)
    {
        perror("Create sockfd error\n");
        exit(1);
    }

    //����Ȩ��λrootȨ��
    setuid(getuid());

    //�ı����buf�������Ĵ�С
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));

    //���sockaddr_in dest_addr
    bzero(&dest_addr,sizeof(dest_addr));

    //����dest_addr
    dest_addr.sin_family = AF_INET;
    
    //����DNS��������
    if((inaddr = inet_addr(argv[1])) == INADDR_NONE)  //�ж��Ƿ�����Ч��IP��ַ
    {
        //������������
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

    //��ȡ��ǰ����pid
    pid = getpid();

    //����
    send_packet(sockfd,pid,dest_addr);

    //�հ�
    recv_packet(sockfd,pid);

    statistics(SIGALRM);

    //�ر��׽���
    close(sockfd);

    return 0;
}
