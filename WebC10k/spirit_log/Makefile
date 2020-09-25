CC=gcc
CC=gcc
CFLAGS = -Wall -g
INCLUDES = -I../include/
LIBS = -lpthread
SRCS =  spirit_log.c test.c
OBJS = $(SRCS:.c=.o)
MAIN = libslog.so
LFLAGS = -L../lib -Wl,-rpath=../lib

.PHONY: depend clean

all: $(MAIN)
	@echo library slog is built
	@cp libslog.so ../lib

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ 

clean:
	rm -f *.o *.a

$(MAIN): $(OBJS)
	$(CC) $(LFLAGS) $(LIBS) -shared -o $(MAIN) $(OBJS)


depend: $(SRCS)
	makedepend $(INCLUDES) $^