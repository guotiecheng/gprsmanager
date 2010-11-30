PRJ_PATH := $(shell pwd)
CFLAGS+=-I${PRJ_PATH} 
LIBS= -L${PRJ_PATH}/libs 

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

LIB_NAME=$(shell basename ${PWD})
TARGET=libvoice.a

all: module_lib common_lib config_lib hal_lib proxy install

#%.o : %.c
proxy:
	@echo "nc_mobiled compile *****************";
#	$(AR) -rcu ${TARGET} obj/*.o
	${CROSS_COMPILE}$(CC) -c  *.c $< $(CFLAGS) 

module_lib:	
	@make --no-print-directory -Cmodule/ -fmakefile 

common_lib:	
	@make --no-print-directory -Ccommon/ -fmakefile 

config_lib:	
	@make --no-print-directory -Cconfig/ -fmakefile 

hal_lib:	
	@make --no-print-directory -Chal/ -fmakefile 

install:
	cp *.o obj
	cp ./obj/*.o ../tmp

clean:
	@make --no-print-directory -Cmodule/ -fmakefile clean
	@make --no-print-directory -Ccommon/ -fmakefile clean
	@make --no-print-directory -Cconfig/ -fmakefile clean
	@make --no-print-directory -Chal/ -fmakefile clean
	rm -rf *.o *.gdb *.a
	rm -rf obj/*.o

