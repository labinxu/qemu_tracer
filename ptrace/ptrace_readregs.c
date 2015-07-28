#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <memory.h>
int main()
{
    pid_t child_pid;
    long int orig_rax;
    //long int params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct regs;
    child_pid = fork();
    if(child_pid == 0 )
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else
    {
        while(1)
        {
            wait(&status);
            if(WIFEXITED(status))
            {
                break;
            }

            orig_rax = ptrace(PTRACE_PEEKUSER, child_pid, 8 * ORIG_RAX, NULL);
            if(orig_rax == SYS_write)
            {
                if(insyscall==0){
                    insyscall = 1;
                    ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
                    printf("Write called with %ld, %ld, %ld\n", regs.rbx, regs.rcx, regs.rdx);
                }
            }
            else
            {
                char data[8];
                memset(data, 0, 8);
                long int rex = ptrace(PTRACE_PEEKUSER, child_pid, RAX*8, data);
                printf("Wirte returned with %ld, %s\n", rex, data);
                insyscall = 0;
            }
            ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);

        }
    }
    return 0;

}
