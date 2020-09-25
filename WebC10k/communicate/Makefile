CC=gcc
CFLAGS = -Wall -g
INCLUDES = -I../include/
LIBS = -lpthread -lslog -lm -lmysqlclient
LFLAGS = -L../lib -Wl,-rpath=../lib
SRCS =  server.c thread_pool.c reactor.c accepter_eh.c signal_handler.c
OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDES) -c $^ -o $@ 
server: $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) $(LFLAGS)
clean:
	rm -f *.o *.a server core
