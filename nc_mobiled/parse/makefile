
PWD := $(shell pwd)
PRJ_PATH := $(shell dirname ${PWD})
CFLAGS+=-I${PRJ_PATH} 
LIBS= -L${PRJ_PATH}/libs 

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

LIB_NAME=$(shell basename ${PWD})
TARGET=lib${LIB_NAME}.a

ALL: proxy
#%.o : %.c
proxy:
	${CROSS_COMPILE}$(CC) -c *.c $< $(CFLAGS)
	@cp *.o $(PRJ_PATH)/tmp

clean:
	rm -rf *.o *.gdb *.a

