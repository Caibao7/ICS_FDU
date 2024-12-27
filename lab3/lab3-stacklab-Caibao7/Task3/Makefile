CC = gcc
CFLAGS = -no-pie -O0 -g

.PHONY: all

all: clean program

program: context.o context_asm.o main.o
	@$(CC) $(CFLAGS) -o program context.o context_asm.o main.o

context.o: context.c
	@$(CC) $(CFLAGS) -c context.c

context_asm.o: context_asm.S
	@$(CC) $(CFLAGS) -c context_asm.S

main.o: main.c
	@$(CC) $(CFLAGS) -c main.c

clean:
	@rm -f program context.o context_asm.o main.o
