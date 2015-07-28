#include<sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/reg.h>
#include <stdio.h>


int main()
{
    pid_t child;
    const int long_size = sizeof(long);
    printf("starting\n");
    child = fork();
    if(child==0)
    {
        printf("child process\n");
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
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
        int start = 0;
        printf("sys_write %d\n", SYS_write);
        long ins;
        while(1)
        {
            wait(&status);
            if(WIFEXITED(status))
            {
                printf("child process exited\n");
                break;
            }
            ptrace(PTRACE_GETREGS, child, NULL, &regs);

            if(start == 1)
            {
                ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
                //printf("EIP:%lx executed:%lx\n", regs.eip,ins);
            }
            printf("EIP:%lx instruction: %ld RAX %ld \n", regs.eip, ins, regs.orig_eax);
            if(regs.orig_eax == SYS_write)
            {

                printf("call sys_write\n");
                start = 1;
                ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
            }
            else
            {
                printf("Execute syscall ptrace\n");
                ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            }
        }
    }
    return 0;

}
