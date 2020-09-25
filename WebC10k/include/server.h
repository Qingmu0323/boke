#define PORT 4321
#define FILE_PORT 4322
//connect user num
#define BACKLOG 64
#define MAXRECVLEN 6000
#define MAX_EVENT_NUM 50
#define TIMESLOT 3


int init_server(int *listenfd,int *epollfd);