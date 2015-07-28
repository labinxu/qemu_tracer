#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <sys/syscall.h>

int main()
{
    pid_t child_pid;
    long orig_rax;
    long int params[3];
    int status;
    int insyscall = 0;
    child_pid = fork();
    if(child_pid == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else
    {
        printf("SYS_write %d\n", SYS_write);
        while(1)
        {
            wait(&status);
            if(WIFEXITED(status))
            {
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, child_pid, 8*ORIG_RAX, NULL);
            if(orig_rax == SYS_write)
            {
                if(insyscall == 0)
                {
                    //syscall entry
                    insyscall = 1;
                    params[0] = ptrace(PTRACE_PEEKUSER, child_pid, 8* RBX, NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER, child_pid, 8 * RCX, NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER, child_pid, 8 * RDX, NULL);
                    printf("Write called with %d ,%ld,%ld, %ld\n",SYS_write, params[0],params[1],params[2]);
                }
            }
            else
            {
                //long int rax =
                ptrace(PTRACE_PEEKUSER, child_pid, 8*RAX,NULL);
                //printf("Write returned With %ld\n", rax);
                insyscall=0;
            }
            ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);

        }
    }
    return 0;

}
