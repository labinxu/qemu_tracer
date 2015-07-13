#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include "debuglib.h"

extern char* strsignal(int);

int main(int argc, char**argv)
{
    pid_t child_pid;
    if(argc < 2){
        fprintf(stderr, "Expected a program address\n");
    }
    child_pid = fork();
    if(child_pid == 0)
    {
        //run_target(argv[1]);
    }
    else if (child_pid > 0)
    {
        //run_debugger(child_pid, argv[2]);
    }
    else
    {
        perror("fork error\n");
        return -1;
    }
    return 0;
}
