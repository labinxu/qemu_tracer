#include<sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/reg.h>
#include <stdio.h>
#include "debuglib.h"


int main(int argc, char **argv)
{
    pid_t child;
    const int long_size = sizeof(long);
    printf("starting\n");
    child = fork();
    if(child==0)
    {
        printf("child process\n");
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        if(argc ==2)
        {
            execl(argv[1], argv[1], NULL);
        }
        execl("./hello","hello", NULL);
    }
    else
    {
        printf("parents process\n");
        int status;
        union u{
            long val;
            char chars[long_size];
        } data;

        struct user_regs_struct regs;
        while(1)
        {
            wait(&status);
            if(WIFEXITED(status))
            {
                printf("child process exited\n");
                break;
            }
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            long ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
            printf("EIP:%lx instruction: %lx RAX %ld \n", regs.rip, ins, regs.orig_rax);
            getchar();
            ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
        }
        printf("Main prcess exited\n");
    }
    return 0;

}
 
