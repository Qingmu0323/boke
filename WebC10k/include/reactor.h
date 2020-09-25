#ifndef REACTOR_H
#define REACTOR_H

#define MAX_USERS 100
#define EVENT_HANDLER_STRUCT_ERR -1
#define FIND_EH_ERR -2
#define INVIALD_POSITION -3
#include "header.h"
#include "thread_pool.h"
typedef struct handle_event_msg handle_event_msg_t;
typedef struct reactor reactor_t;

//针对某个socket fd 的事件处理结点
typedef struct event_handler
{
	int fd;
	reactor_t *reactor;
	void (*handle_event)(handle_event_msg_t *handle_event_msg);
}event_handler_t;


typedef struct reactor_core
{
	int epoll_fd;
	size_t current_idx;
	event_handler_t *ehs[MAX_USERS-1];
}reactor_core_t;

typedef struct reactor
{
	int (*add_eh)(struct reactor *self,event_handler_t *eh);
	int (*rm_eh)(struct reactor *self,int fd);
	int (*event_loop)(struct reactor *self);
	reactor_core_t * core;
	thpool_t * thpool;
}reactor_t;

typedef void (*event_callback_fn)(handle_event_msg_t);
#endif