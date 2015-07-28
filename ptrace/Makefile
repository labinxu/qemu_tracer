all:PART1 PART2 READ_REGS REVERSE_STR SINGLE_STEP
CC := gcc
CXX := g++
MECHINE_ARCH = -m32
PART1:
	${CC} ${MECHINE_ARCH} -Wall -Wextra -o part1 part1.c
PART2:
	${CC} -Wall -Wextra -o part2 part2.c
READ_REGS:
	${CC} -Wall -Wextra -o read_regs ptrace_readregs.c
REVERSE_STR:
	${CXX} -g -Wall -Wextra -o reverse revert_str.cpp
SINGLE_STEP:
	${CXX} -m32 -g -Wall -Wextra -o single_step single_step.c
clean:
	rm -f reverse part1 part2 read_regs *.o single_step tracer *.out *~

