#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include "spirit_log.h"

void test_hello()
{
	int res = 0;
	slog_debug("debug %d %d\n",res,res);
	slog_debug("debug\n");
	slog_debug("debug\n");
	slog_debug("debug\n");
	slog_debug("debug\n");
	slog_info("info\n");
	slog_info("info\n");
	slog_info("info\n");
	slog_info("info\n");
	slog_info("info\n");
	slog_error("error\n");
	slog_error("error\n");
	slog_error("error\n");
	slog_error("error\n");
	slog_error("error\n");

}
void * work(int tag)
{
	switch(tag)
	{
		case 1:
			slog_debug("aaaaaaaaaaaa %d",tag);
			break;
		case 2:
			slog_debug("sssssssssss%d",tag);
			break;
		case 3:
			slog_debug("wwwwwww%d",tag);
			break;
	}
	slog_debug("test thread %d",tag);
	return;
}

void test_thread()
{
	printf("test thread begin\n");
	int i,j;
	int fork_num = 4;
	int thread_num = 4;
	pid_t pid;
	for (i = 0; i < fork_num; ++i)
	{
		pid =  fork();
		if (pid<0)
		{
			printf("fork error \n");
		}
		else if (pid == 0)
		{
			pthread_t tid[thread_num];

			for (j = 0; j < thread_num; j++) { 
				pthread_create(&(tid[j]), NULL, work, j);
			}
			for (j = 0; j < thread_num; j++) { 
				pthread_join(tid[j], NULL);
			}
			return;
		}
	}

	for (i = 0; i < 4; i++) 
	{
		pid = wait(NULL);
	}
	return;
}
int main(int argc, char const *argv[])
{
	int res;
	res = log_init();
	if (res)
	{
		printf("init error %d \n",res);
	}
	test_thread();

	return 0;
}