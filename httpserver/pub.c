/******************************************************************************

                  版权所有 (C), 2017-2017, chandler

 ******************************************************************************
  文 件 名   : pub.c
  版 本 号   : 初稿
  作    者   : chandler
  生成日期   : 2018年5月4日
  最近修改   :
  功能描述   : 服务器初始化
  函数列表   :
*
*       1.                get_filetype
*       2.                get_file_content
*       3.                save_log
*       4.                setdaemon
*       5.                socket_accept
*       6.                socket_create
*
#include<stdlib.h>
  修改历史   :
  1.日    期   : 2018年5月4日
    作    者   : chandler
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/



#include<pthread.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<arpa/inet.h>

#include"pub.h"
#include"thread_work.h"
char LOGBUF[1024];
void save_log(char *buf)
{
      FILE *fp = fopen("log.txt","a+");  
      fputs(buf,fp);  
      fclose(fp);  
}




/*****************************************************************************
 函 数 名  : setdaemon
 功能描述  : 设置守护进程
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年12月3日
    作    者   : chandler
    修改内容   : 新生成函数

*****************************************************************************/
void setdaemon() //璁剧疆涓哄畧鎶よ繘绋?
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"fork failed %s\n", strerror(errno));
		save_log(LOGBUF);
		exit (EXIT_FAILURE);
	}
	if (pid > 0)
	{
		exit (EXIT_SUCCESS);
	}

	if ((sid = setsid()) < 0)
	{
		printf("setsid failed %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}
  /*if (chdir("/") < 0)
	 {
	 printf("chdir failed %s\n", strerror(errno));
	 exit(EXIT_FAILURE);
	 }*/
	 umask(0);
	 close(STDIN_FILENO);
	 close(STDOUT_FILENO);
	 close(STDERR_FILENO);

}

const char *get_filetype(const char *filename) //鏍规嵁鎵╁睍鍚嶈繑鍥炴枃浠剁被鍨嬫弿杩?
{
	////////////寰楀埌鏂囦欢鎵╁睍鍚?///////////////////
	char sExt[32];
	const char *p_start=filename;
	memset(sExt, 0, sizeof(sExt));
	while(*p_start)
	{
		if (*p_start == '.')
		{
			p_start++;
			strncpy(sExt, p_start, sizeof(sExt));
			break;
		}
		p_start++;
	}

	////////鏍规嵁鎵╁睍鍚嶈繑鍥炵浉搴旀弿杩?///////////////////

	if (strncmp(sExt, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(sExt, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(sExt, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(sExt, "jpg", 3) == 0)
		return "image/jpeg";

	if (strncmp(sExt, "avi", 3) == 0)
		return "video/avi";

	if (strncmp(sExt, "css", 3) == 0)
		return "text/css";

	if (strncmp(sExt, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "exe", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "dtd", 3) == 0)
		return "text/xml";

	if (strncmp(sExt, "mp3", 3) == 0)
		return "audio/mp3";

	if (strncmp(sExt, "mpg", 3) == 0)
		return "video/mpg";

	if (strncmp(sExt, "png", 3) == 0)
		return "image/png";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/vnd.ms-powerpoint";

	if (strncmp(sExt, "xls", 3) == 0)
		return "application/vnd.ms-excel";

	if (strncmp(sExt, "doc", 3) == 0)
		return "application/msword";

	if (strncmp(sExt, "mp4", 3) == 0)
		return "video/mpeg4";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/x-ppt";

	if (strncmp(sExt, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(sExt, "wmv", 3) == 0)
		return "video/x-ms-wmv";

	return "text/html";
}


/*****************************************************************************
 函 数 名  : socket_create
 功能描述  : socket创建
 输入参数  : int port  
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年5月4日
    作    者   : chandler
    修改内容   : 新生成函数

*****************************************************************************/
int socket_create(int port)
{
	int st = socket(AF_INET, SOCK_STREAM, 0);
	int on =1;
	if (st == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:socker error %s\n", __FILE__, __LINE__, strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"setsockopt failed %s\n", strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_port = htons(port); //鎸囧畾涓?涓鍙ｅ彿骞跺皢hosts瀛楄妭鍨嬩紶鍖栨垚Inet鍨嬪瓧鑺傚瀷锛堝ぇ绔垨鎴栬?呭皬绔棶棰橈級
	sockaddr.sin_family = AF_INET;	//璁剧疆缁撴瀯绫诲瀷涓篢CP/IP
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//鏈嶅姟绔槸绛夊緟鍒汉鏉ヨ繛锛屼笉闇?瑕佹壘璋佺殑ip
	//杩欓噷鍐欎竴涓暱閲廔NADDR_ANY琛ㄧずserver涓婃墍鏈塱p锛岃繖涓竴涓猻erver鍙兘鏈夊涓猧p鍦板潃锛屽洜涓哄彲鑳芥湁澶氬潡缃戝崱
	if (bind(st, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:bind error %s \n", __FILE__, __LINE__, strerror(errno));
		save_log(LOGBUF);
		return 0;
	}

	if (listen(st, 100) == -1) // 	鏈嶅姟绔紑濮嬬洃鍚?
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:listen failture %s\n", __FILE__, __LINE__,
				strerror(errno));
				save_log(LOGBUF);
		return 0;
	}
	printf("start server success!\n");
	return st;

}
int socket_accept(int st)
{
	int client_st;
	struct sockaddr_in client_sockaddr;
	socklen_t len = sizeof(client_sockaddr);

	pthread_t thrd_t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //鍒濆鍖栫嚎绋嬩负鍙垎绂荤殑
	memset(&client_sockaddr, 0, sizeof(client_sockaddr));

	while (1)
	{
		client_st = accept(st, (struct sockaddr *) &client_sockaddr, &len);
		if (client_st == -1)
		{
			memset(LOGBUF,0,sizeof(LOGBUF));
			sprintf(LOGBUF,"%s,%d:accept failture %s \n", __FILE__, __LINE__,
					strerror(errno));
			save_log(LOGBUF);
			return 0;
		} else
		{
			int *tmp = (int *) malloc(sizeof(int));
			*tmp = client_st;
			pthread_create(&thrd_t, &attr, http_thread, tmp);
		}

	}
	pthread_destory(&attr);//閲婃斁璧勬簮
}

int  get_file_content(const char *file_name, char **content) // 寰楀埌鏂囦欢鍐呭
{
	int  file_length = 0;
	FILE *fp = NULL;

	if (file_name == NULL)
	{
		return file_length;
	}
    //数据库读写函数来代替
	fp = fopen(file_name, "rb");

	if (fp == NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"file name: %s,%s,%d:open file failture %s \n",file_name, __FILE__, __LINE__,
				strerror(errno));
		save_log(LOGBUF);
		return file_length;
	}

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	rewind(fp);

	*content = (char *) malloc(file_length);
	if (*content == NULL)
	{
		memset(LOGBUF,0,sizeof(LOGBUF));
		sprintf(LOGBUF,"%s,%d:malloc failture %s \n", __FILE__, __LINE__,
				strerror(errno));
		save_log(LOGBUF);
		return 0;
	}
	fread(*content, file_length, 1, fp);
	fclose(fp);

	return file_length;
}
