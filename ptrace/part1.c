#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <stdlib.h>
#include <sys/syscall.h>
int main()
{
    pid_t child_pid;
    long int orig_eax;
    child_pid = fork();
    if(child_pid==0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else
    {
      int toggle =0;
      long int params[4];
      int status;
      printf("sys write %d read %d\n", SYS_write,SYS_read);
      while(1)
      {
          wait(&status);
          if(WIFEXITED(status))
          {
              break;
          }
          orig_eax = ptrace(PTRACE_PEEKUSER, child_pid, 4 * ORIG_EAX, NULL);

        if(orig_eax == SYS_write)
          {
            if(toggle ==0)
                {
                    toggle = 1;
                    params[0] = ptrace(PTRACE_PEEKUSER,child_pid, 4 * EBX,NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER,child_pid, 4 * ECX,NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER,child_pid, 4 * EDX,NULL);
                    printf("para1 %ld, %ld, %ld", params[0], params[1], params[2]);
                }
                else
                {
                    toggle = 0;
                }
            }
        // printf("The child made a system call %ld\n", orig_eax);
        ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
      }
    }
    return 0;
}
