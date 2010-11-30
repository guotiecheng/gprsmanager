OBJFILES := $(patsubst %.c,%.o,$(wildcard *.c))

#all: clean ${OBJFILES}### Change by WENJING for quick compile
all: ${OBJFILES}

%.o: %.c
	${CROSS_COMPILE}$(CC) $(CFLAGS) -c -o $@ $<
	cp *.o  ../../obj

clean:
	@rm -f *.o *.a
