#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

all: test-heat heatgen 

test-heat: test-heat.c heatsim.o cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o test-heat test-heat.c cachelab.c heatsim.o

heatgen: heatgen.c heatsim.o cachelab.c
	$(CC) $(CFLAGS) -O0 -o heatgen heatgen.c heatsim.o cachelab.c

heatsim.o: heatsim.c
	$(CC) $(CFLAGS) -O0 -c heatsim.c

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f test-heat heatgen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
	rm -rf trace.tmp
