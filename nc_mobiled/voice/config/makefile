PRJ_PATH := $(shell pwd)
CFLAGS+=-I${PRJ_PATH} 
LIBS= -L${PRJ_PATH}/libs 

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

LIB_NAME=$(shell basename ${PWD})
TARGET=lib${LIB_NAME}.a

all: proxy install 

#%.o : %.c
proxy:
	@echo "nc_mobiled voice compile *****************";
	${CROSS_COMPILE}$(CC) -c  *.c $< $(CFLAGS) 

install:
	cp *.o ../obj

clean:
	rm -rf *.o *.gdb *.a

