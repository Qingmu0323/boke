CC=gcc
CFLAGS = -Wall -g
INCLUDES = -I../include/
LIBS = -lpthread
SRCS =  clent.c thread_pool.c
OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ 
test: $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS)
clean:
	rm -f *.o *.a