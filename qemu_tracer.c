#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/reg.h>
#include "debuglib.h"
#include <sys/user.h>
extern char* strsignal(int);
void run_debugger(pid_t child_pid)
{
    int wait_status;
    struct user_regs_struct regs;
    procmsg("debugger started %d\n", child_pid);
    wait(&wait_status);
    ptrace(PTRACE_GETREGS, child_pid,0, &regs);
}
int main(int argc, char**argv)
{
    pid_t child_pid;
    if(argc < 2){
        fprintf(stderr, "Expected a program address\n");
        return -1;
    }
    child_pid = fork();
    if(child_pid == 0)
    {
        run_target(argv[1]);
    }
    else if (child_pid > 0)
    {
        run_debugger(child_pid);
    }
    else
    {
        perror("fork error\n");
        return -1;
    }
    return 0;
}
