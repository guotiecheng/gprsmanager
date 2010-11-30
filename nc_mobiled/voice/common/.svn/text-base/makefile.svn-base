PRJ_PATH := $(shell pwd)
CFLAGS+=-I${PRJ_PATH} 
LIBS= -L${PRJ_PATH}/libs 

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

LIB_NAME=$(shell basename ${PWD})
TARGET=lib${LIB_NAME}.a

all: proxy expat_lib install 

#%.o : %.c
proxy:
	@echo "nc_mobiled voice compile *****************";
	${CROSS_COMPILE}$(CC) -c  *.c $< $(CFLAGS) 

expat_lib:
	@make --no-print-directory -Cexpat/ -fmakefile 

install:
	cp *.o ../obj

clean:
	@make --no-print-directory -Cexpat/ -fmakefile clean
	rm -rf *.o *.gdb *.a

