LBITS := $(shell getconf LONG_BIT)
CC := gcc
CC_FLAGS := -Wall -Wextra -std=c99

all: qemu_tracer

qemu_tracer:
	${CC} ${CC_FLAGS} -c debuglib.c
	${CC} ${CC_FLAGS} -c qemu_tracer.c
	${CC} -o qemu_tracer qemu_tracer.o debuglib.o

manual_clean:
	rm -f *.o hello bp_manual qemu_tracer

.PHONY : clean
clean : manual_clean
