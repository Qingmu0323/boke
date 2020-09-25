#ifndef LOG_H
#define LOG_H 

#define LOG_BUFFER_MAX_LENGTH 1024
#define DEBUG_LOG "debug.log"
#define INFO_LOG "info.log"
#define ERROR_LOG "error.log"
#define SLOG_VERSION "0.1"

#define SLOG_LEVEL_DEBUG 20
#define SLOG_LEVEL_INFO 40
#define SLOG_LEVEL_ERROR 60

// typedef enum {
// 	SLOG_LEVEL_DEBUG = 20,
// 	SLOG_LEVEL_INFO = 40,
// 	SLOG_LEVEL_ERROR = 60
// } slog_level; 


typedef struct {
	int init_version;
	char msg_buf[LOG_BUFFER_MAX_LENGTH+1];
} slog_thread_t;


	
// int fetch_thread_buffer(slog_thread_t **thread_buffer);
int slog(int tag,const char *file, size_t filelen, const char *func, 
	size_t funclen,long line,const char* format,...);

#define slog_debug(...)\
	slog(SLOG_LEVEL_DEBUG,__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__,\
		__VA_ARGS__)

#define slog_info(...)\
	slog(SLOG_LEVEL_INFO,__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__,\
		__VA_ARGS__)

#define slog_error(...)\
	slog(SLOG_LEVEL_ERROR,__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__,\
		__VA_ARGS__)

#endif