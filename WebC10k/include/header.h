#ifndef HEAD_H
#define HEAD_H 

#define MAX_LENGTH 5*1024
#include "reactor.h"
#include <stdint.h>
typedef struct event_handler event_handler_t;


typedef struct common_package
{
	int src_id;
	int des_id;
	char msg_type;
	int size;
	int total;
	char body[4096];
}common_package_t;


typedef struct handler_msg
{
	struct common_package *pack;
	int fd;
	char ip[20];
}handler_msg_t;


typedef struct handle_event_msg
{
	event_handler_t *eh;
	uint32_t e;

}handle_event_msg_t;

typedef struct receive_msg
{
	int fd;
	char ip[20];
}receive_msg_t;

// typedef struct handler_msg handler_msg_t;

// msg_type
#define OPERATE 0x80
#define COM_MSG 0x60
#define COM_PIC 0x61
#define COM_VOICE 0x62
#define COM_VIDEO 0x63
#define KEEP_LIVE 0xC0
#define OFF_MSG 0x04
#define RESUME_BREAKPOINT_MSG 0x08
#define OFF_MSG_SUCCESS 0x0C

//src_id
#define REGISTE_USER 20000
#define LOGIN_USER 20001
//des_id
#define BROADCAST_MSG 30000

#endif
