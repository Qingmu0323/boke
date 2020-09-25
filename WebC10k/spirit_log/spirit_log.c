#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include "spirit_log.h"

static pthread_rwlock_t log_env_lock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_rwlock_t debug_log_lock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_rwlock_t info_log_lock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_rwlock_t error_log_lock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_key_t slog_thread_key;

static void slog_clean_thread_key(void)
{
	slog_thread_t *a_thread;
	a_thread = pthread_getspecific(slog_thread_key);
	if (!a_thread) 
		return;
	free(a_thread);
	return;
}

int log_init()
{
	printf("---------log_init start--------- \n");
	printf("start time \n");

	int res;
	res = pthread_rwlock_wrlock(&log_env_lock);

	if (res) 
	{
		printf("pthread_rwlock_wrlock fail, rc[%d]\n", res);
		return -1;
	}

	res = pthread_key_create(&slog_thread_key,NULL);
	if (res)
	{
		slog_error("pthread_key_create error\n");
	}

	res = atexit(slog_clean_thread_key);

	printf("------log_init success end------\n");
	res = pthread_rwlock_unlock(&log_env_lock);
	if (res) {
		printf("pthread_rwlock_unlock fail, rc=[%d]\n", res);
		return -1;
	}
	return 0;
}



#define fetch_thread_buffer(thread_buffer) do { \
	thread_buffer = pthread_getspecific(slog_thread_key);\
	if (!(thread_buffer))\
	{\
		thread_buffer = calloc(1,sizeof(slog_thread_t));\
		if (!thread_buffer)\
		{\
			slog_error("fetch_thread_buffer calloc error\n");\
		}\
		int res = 0;\
		res = pthread_setspecific(slog_thread_key, thread_buffer);  \
		if (res)\
		{\
			slog_error("fetch_thread_buffer pthread_setspecific error \n");\
		}\
	}\
}while(0)


int slog(int tag,const char *file, size_t filelen, const char *func, 
	size_t funclen,long line,const char* format,...)
{
	//res and buffer should be locked first
	slog_thread_t *thread_buffer;
	fetch_thread_buffer(thread_buffer);

    va_list arg;
    FILE *debug_log_file, *info_log_file, *error_log_file;
    int res;

    va_start (arg, format);
    vsnprintf(thread_buffer->msg_buf, LOG_BUFFER_MAX_LENGTH, format, arg);
    va_end (arg);
    switch(tag)
    {
    	case SLOG_LEVEL_DEBUG:
			res = pthread_rwlock_wrlock(&debug_log_lock);

			if (res) 
			{
				slog_error("SLOG_LEVEL_DEBUG: pthread_rwlock_wrlock fail, rc[%d]\n", res);
				return -1;
			}
	    	debug_log_file = fopen(DEBUG_LOG,"a");
	    	if (!debug_log_file)
	    	{
	    		slog_error("SLOG_LEVEL_DEBUG: open file error %s/n",DEBUG_LOG);
	    	}
	    	fprintf(debug_log_file, "[%s][%s] pid is [%d] filename is [%s],fun is [%s], at line[%d] , debug_info: %s\n",__DATE__,__TIME__,getpid(),file, func,line,thread_buffer->msg_buf);
	    	fclose(debug_log_file);
	    	res = pthread_rwlock_unlock(&debug_log_lock);
			if (res) 
			{
				slog_error("SLOG_LEVEL_DEBUG: pthread_rwlock_unlock fail, rc=[%d]\n", res);
				return -1;
			}
    	break;

    	case SLOG_LEVEL_INFO:
    		res = pthread_rwlock_wrlock(&info_log_lock);
			if (res) 
			{
				slog_error("SLOG_LEVEL_INFO: pthread_rwlock_wrlock fail, rc[%d]/n", res);
				return -1;
			}
	    	info_log_file = fopen(INFO_LOG,"a");
	    	if (!info_log_file)
	    	{
	    		slog_error("open file error %s/n",INFO_LOG);
	    	}
	    	fprintf(info_log_file, "[%s][%s] pid is [%d] filename is [%s],fun is [%s], at line [%d] , info: %s",__DATE__,__TIME__,getpid(),file, func,line,
	    	thread_buffer->msg_buf);
	    	fclose(info_log_file);
	    	res = pthread_rwlock_unlock(&info_log_lock);
			if (res) 
			{
				slog_error("SLOG_LEVEL_INFO: pthread_rwlock_unlock fail, rc=[%d]/n", res);
				return -1;
			}
    	break;
    	case SLOG_LEVEL_ERROR:
    		res = pthread_rwlock_wrlock(&error_log_lock);
			if (res) 
			{
				slog_error("SLOG_LEVEL_ERROR: pthread_rwlock_wrlock fail, rc[%d]/n", res);
				return -1;
			}
	    	error_log_file = fopen(ERROR_LOG,"a");
	    	if (!error_log_file)
	    	{
	    		slog_error("open file error %s/n",ERROR_LOG);
	    	}
	    	fprintf(error_log_file, "[%s][%s] pid is [%d] filename is [%s],fun is [%s], at line [%d] , error_info: %s\n",__DATE__,__TIME__,getpid(),file, func,line,thread_buffer->msg_buf);
	    	fclose(error_log_file);
	    	res = pthread_rwlock_unlock(&error_log_lock);
			if (res) 
			{
				slog_error("SLOG_LEVEL_ERROR: pthread_rwlock_unlock fail, rc=[%d]/n", res);
				return -1;
			}

    	break;
 		default:
 			slog_error("error slog tag\n");
    }

    return 0;
}


// int fetch_thread_buffer(slog_thread_t *thread_buffer)
// {
// 	thread_buffer = pthread_getspecific(slog_thread_key);
// 	if (!(thread_buffer))
// 	{
// 		thread_buffer = calloc(1,sizeof(slog_thread_t));
// 		if (!thread_buffer)
// 		{
// 			slog_error("fetch_thread_buffer error\n");
// 			return -1;
// 		}
// 	}
// 	return 0;
// }
