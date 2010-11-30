PRJ_PATH := $(shell pwd)
CFLAGS+=-I${PRJ_PATH} 
LIBS= -L${PRJ_PATH}/libs 

VPATH= .
SRCS = $(wildcard ${VPATH}/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

LIB_NAME=$(shell basename ${PWD})
TARGET=lib${LIB_NAME}.a

all: proxy common_lib comport_lib gprs_lib ppp_lib parse_lib sms_lib voice_lib install 

#%.o : %.c
proxy:
	@echo "nc_mobiled compile *****************";
	${CROSS_COMPILE}$(CC) -c  *.c $< $(CFLAGS) 

common_lib:	
	@make --no-print-directory -Ccommon/ -fmakefile 

comport_lib:	
	@make --no-print-directory -Ccomport/ -fmakefile 

gprs_lib:	
	@make --no-print-directory -Cgprs/ -fmakefile 

ppp_lib:	
	@make --no-print-directory -Cppp/ -fmakefile 

parse_lib:	
	@make --no-print-directory -Cparse/ -fmakefile 

sms_lib:	
	@make --no-print-directory -Csms/ -fmakefile 

voice_lib:	
	@make --no-print-directory -Cvoice/ -fmakefile 

install:
	cp *.o ${PRJ_PATH}/tmp

clean:
	@make --no-print-directory -Ccommon/ -fmakefile clean
	@make --no-print-directory -Ccomport/ -fmakefile clean
	@make --no-print-directory -Cgprs/ -fmakefile clean
	@make --no-print-directory -Cppp/ -fmakefile clean
	@make --no-print-directory -Cparse/ -fmakefile clean
	@make --no-print-directory -Csms/ -fmakefile clean
	@make --no-print-directory -Cvoice/ -fmakefile clean
	rm -rf *.o *.gdb *.a
	rm -rf tmp/*

