#include "reactor.h"
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h> //malloc


event_handler_t *find_eh(reactor_core_t *core,int fd,int *index)
{
	int i = 0;
	event_handler_t *tmp;
	for (i; i < core->current_idx; ++i)
	{
		if ((tmp = core->ehs[i]) && (tmp->fd == fd) )
		{
			if (index)
			{
				*index = i;
			}
			printf("find_eh fd is %d\n",fd );			
			return tmp;
		}
	}
	return NULL;
}

int add_eh(reactor_t *self,event_handler_t *eh)
{
	int result = 0;
	struct epoll_event ee;
	memset(&ee, 0, sizeof(ee));
	ee.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
	ee.data.fd = eh->fd;
	epoll_ctl(self->core->epoll_fd, EPOLL_CTL_ADD, eh->fd, &ee);

	if (eh->fd < 0 || eh->handle_event == NULL)
	{
		printf("event_handler_t has something null\n");
		return EVENT_HANDLER_STRUCT_ERR;
	}
    
	if (self->core->current_idx < MAX_USERS)
	{
		self->core->ehs[self->core->current_idx ++] = eh;
		printf("current_idx is %d fd is %d\n",self->core->current_idx,eh->fd);
	}
	return result;
}



int rm_eh(reactor_t *self,int fd)
{
	int where;
	event_handler_t * eh = find_eh(self->core,fd,&where);
	if (eh)
	{
		printf("can not find handler_event \n");
		return FIND_EH_ERR;
	}
	if (where >= self->core->current_idx)
	{
		printf("invaild index \n");
		return INVIALD_POSITION;
	}
	self->core->ehs[where] = self->core->ehs[self->core->current_idx];
	self->core->current_idx-- ;
	epoll_ctl(self->core->epoll_fd, EPOLL_CTL_DEL, eh->fd, 0);
	close(eh->fd);
	free(eh);
}

int event_loop(reactor_t *self)
{
	printf("event_loop\n");
	int i,num;
	struct epoll_event ees[MAX_USERS];
	event_handler_t *eh;
	while(1)
	{
		num = epoll_wait(self->core->epoll_fd,ees,MAX_USERS,-1);
		for (i = 0; i < num; ++i)
		{
			printf("event_loop epoll_wait for %d\n",num);
			eh = find_eh(self->core,ees[i].data.fd,0);
			if (ees[i].events & EPOLLRDHUP)
			{
				// user_offine(ees[i].data.fd);
				printf("user down\n");
			}
			if (eh)
			{
				printf("loop once\n");
				handle_event_msg_t *handle_event_msg = malloc(sizeof(handle_event_msg_t));
				handle_event_msg->eh = eh;
				handle_event_msg->e = ees[i].events;
				// if (handle_event_msg->e)
				// {
				// 	/* code */
				// }
				// eh->handle_event(handle_event_msg);
				//添加当前事件到任务池 
				thpool_add_work(self->thpool,eh->handle_event,handle_event_msg);
				// printf("thpool_add_work ends\n");
				// free(handle_event_msg);
			}
		}
	}
}


reactor_t *create_reactor(int epoll_fd,thpool_t *thpool)
{
	reactor_t * reactor = malloc(sizeof(reactor_t));
	reactor->add_eh = &add_eh; //向epoll监听的链表上 注册一个监听事件 
	reactor->rm_eh = &rm_eh;
	reactor->event_loop = &event_loop;  // 监听所有事件的到来，并且触发任务管理线程 向任务队列添加任务
	reactor->thpool = thpool;
	reactor->core = malloc(sizeof(reactor_core_t));
	reactor->core->epoll_fd = epoll_fd;
	reactor->core->current_idx = 0;
	reactor->core->ehs[0] = NULL;
	return reactor;
}