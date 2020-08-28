objs =   start.o main.o
CC = arm-linux-gnueabihf-gcc
ledc.bin : $(objs)
	arm-linux-gnueabihf-ld -Timx6ul.lds $^ -o ledc.elf
	arm-linux-gnueabihf-objcopy -O binary -S ledc.elf $@
	arm-linux-gnueabihf-objdump -D -m arm ledc.elf >ledc.dis

start.o:start.s
	$(CC) -g -Wall -nostdlib -c  -o $@  start.s

%.o:%c 
	$(CC) -Wall -nostdlib -c  -o $@ $<

clean:
	rm -rf *.o ledc.dis ledc.bin ledc.elf 

