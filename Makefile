###########################################
# Simple Makefile for HIDAPI test program
#
# Alan Ott
# Signal 11 Software
# 2010-06-01
###########################################

all: mp709

CC=gcc
CXX=g++
COBJS=mp709.o
OBJS=$(COBJS) 
CFLAGS+=-Wall -g -c -std=c99 `pkg-config hidapi-hidraw --cflags`
LIBS=`pkg-config hidapi-hidraw --libs`


mp709: $(OBJS)
	gcc -Wall -g $^ $(LIBS) -o mp709

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) mp709

.PHONY: clean
