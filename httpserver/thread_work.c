/*
 * thread_work.c
 *
 *  Created on: 2015-4-16
 *      Author: hsl
 */


 
#include"thread_work.h"
#include"pub.h"

#include <sys/types.h>
#include <sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
//http æ¶ˆæ¯å¤´
#define HEAD "HTTP/1.0 200 OK\n\
Content-Type: %s\n\
Transfer-Encoding: chunked\n\
Connection: Keep-Alive\n\
Accept-Ranges:bytes\n\
Content-Length:%d\n\n"
//http æ¶ˆæ¯å°¾
#define TAIL "\n\n"
extern char LOGBUF[1024];




/*****************************************************************************
 º¯ Êı Ãû  : get_http_command
 ¹¦ÄÜÃèÊö  : ½âÎöHTTPÇëÇó
 ÊäÈë²ÎÊı  : char *http_msg  
             char *command   
 Êä³ö²ÎÊı  : ÎŞ
 ·µ »Ø Öµ  : void
 µ÷ÓÃº¯Êı  : 
 ±»µ÷º¯Êı  : 
 
 ĞŞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2017Äê11ÔÂ25ÈÕ
    ×÷    Õß   : chandler
    ĞŞ¸ÄÄÚÈİ   : ĞÂÉú³Éº¯Êı

*****************************************************************************/
void get_http_command(char *http_msg, char *command)
{
	char *p_end = http_msg;
	char *p_start = http_msg;
	while (*p_start) //GET /
	{
		if (*p_start == '/')
		{
			break;
		}
		p_start++;
	}
	p_start++;
	p_end = strchr(http_msg, '\n');
	while (p_end != p_start)
	{
		if (*p_end == ' ')
		{
			break;
		}
		p_end--;
	}
	strncpy(command, p_start, p_end - p_start);  //URL

}

//æ ¹æ®ç”¨æˆ·åœ¨GETä¸­çš„è¯·æ±‚ï¼Œç”Ÿæˆç›¸åº”çš„å›å¤å†…å®¹
int make_http_content(const char *command, char **content)
{
	char *file_buf;
	int file_length;
	char headbuf[1024];

	if (command[0] == 0)
	{
		file_length = get_file_content("index.html", &file_buf);
	} else
	{
		file_length = get_file_content(command, &file_buf);
	}
    
	if (file_length == 0)
	{
		return 0;
	}

	memset(headbuf, 0, sizeof(headbuf));
	sprintf(headbuf, HEAD, get_filetype(command), file_length); //è®¾ç½®æ¶ˆæ¯å¤´

	int iheadlen = strlen(headbuf); //å¾—åˆ°æ¶ˆæ¯å¤´é•¿åº¦
	int itaillen = strlen(TAIL); //å¾—åˆ°æ¶ˆæ¯å°¾é•¿åº¦
	int isumlen = iheadlen + file_length + itaillen; //å¾—åˆ°æ¶ˆæ¯æ€»é•¿åº¦
	*content = (char *) malloc(isumlen); //æ ¹æ®æ¶ˆæ¯æ€»é•¿åº¦ï¼ŒåŠ¨æ€åˆ†é…å†…å­˜
	if(*content==NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"malloc failed %s\n", strerror(errno));
		save_log(LOGBUF);
	}
	char *tmp = *content;
	memcpy(tmp, headbuf, iheadlen); //å®‰è£…æ¶ˆæ¯å¤´
	memcpy(&tmp[iheadlen], file_buf, file_length); //å®‰è£…æ¶ˆæ¯ä½“
	memcpy(&tmp[iheadlen] + file_length, TAIL, itaillen); //å®‰è£…æ¶ˆæ¯å°¾
	//printf("headbuf:\n%s", headbuf);
	if (file_buf)
	{
		free(file_buf);
	}
	return isumlen; //è¿”å›æ¶ˆæ¯æ€»é•¿åº¦
}
void *http_thread(void *argc)
{
	//printf("thread begin \n");
	if(argc==NULL)
	{
		return NULL;
	}
	int st = *(int *) argc;
	free((int *)argc);
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	int rc = recv(st, buf, sizeof(buf), 0);
	if (rc <= 0)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"recv failed %s\n", strerror(errno));
		save_log(LOGBUF);
	} else
	{
		//printf("recv:\n%s", buf);
		char command[1024];
		memset(command, 0, sizeof(command));
		get_http_command(buf, command); //å¾—åˆ°http è¯·æ±‚ä¸­ GETåé¢çš„å­—ç¬¦ä¸²
		printf("get:%s \n", command);
		char *content = NULL;
		int ilen = make_http_content(command, &content); //æ ¹æ®ç”¨æˆ·åœ¨GETä¸­çš„è¯·æ±‚ï¼Œç”Ÿæˆç›¸åº”çš„å›å¤å†…å®¹
		if (ilen > 0)
		{
			send(st, content, ilen, 0); //å°†å›å¤çš„å†…å®¹å‘é€ç»™clientç«¯socket
			free(content);
		}
	}
	close(st); //å…³é—­clientç«¯socket
	//printf("thread_is end\n");
	return NULL;

}
