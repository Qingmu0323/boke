/*
accept and receive socket
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h> //malloc
#include <sys/epoll.h>
#include <errno.h>

#include "reactor.h"
#include "server.h"
#include "header.h"


//链接句柄的处理方法
static void package_handler(handle_event_msg_t* handle_event_msg)
{
	printf("package_handler\n");
	int n;
	event_handler_t *self = handle_event_msg->eh;
	uint32_t e = handle_event_msg->e;
	common_package_t *package;
	package = malloc(sizeof(common_package_t));
	receive_msg_t *receive_msg = malloc(sizeof(receive_msg_t));
	if (e & EPOLLRDHUP)
	{
		printf("client down\n");
	}
	else if (e & EPOLLIN)
	{
	    // 做具体的业务逻辑处理
		n = recv(self->fd, package, MAXRECVLEN, 0);
	    if( n <= 0 )
	    {
	        printf("recv err\n");
	        close( self->fd );
	    }
	    printf("package_>body is %s\n",package->body );
	    strcpy(package->body,"asdasdasdasdasd");
	    n = send(self->fd,package,sizeof(common_package_t),0);

	    // receive_msg->fd = self->fd;
	    // strcpy(receive_msg->ip,"127.0.0.1");
	    // reveive_message(receive_msg);

		free(package);
		free(receive_msg);
		free(handle_event_msg);	
	}
	
	
}

event_handler_t* create_package_operater(int fd,reactor_t *r)
{
	printf("create_package_operater\n");
	event_handler_t* eh = malloc(sizeof(event_handler_t));
	eh->fd = fd;
	eh->reactor = r;
	eh->handle_event = &package_handler;

	return eh;
}

static void handle_event(handle_event_msg_t* handle_event_msg)
{
	event_handler_t *self = handle_event_msg->eh;
	uint32_t e = handle_event_msg->e;
	printf("acceptor_handle_event\n");
	int cli_fd = -1;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);

	memset(&cli_addr, 0, sizeof(cli_addr));

	cli_fd = accept(self->fd, (struct sockaddr*) &cli_addr, &cli_addr_len);
	printf("accept fd is %d\n",cli_fd );
    // 创建链接句柄的event_handler
	event_handler_t* ceh = create_package_operater(cli_fd, self->reactor);
	self->reactor->add_eh(self->reactor, ceh);
	printf("handle_event ends\n");
	free(handle_event_msg);
}

event_handler_t* create_acceptor(int fd, reactor_t* r)
{
    // 创建event_handler
	event_handler_t* eh = malloc(sizeof(event_handler_t));
	eh->fd = fd;
	eh->reactor = r;
    //绑定该句柄的处理函数  监听句柄的处理函数
	eh->handle_event = &handle_event;

	return eh;
}