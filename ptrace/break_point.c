#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>


const int long_size = sizeof(long);

void getdata(pid_t child, long addr, char *str, int len)
{
    char *laddr=NULL;

    union u
    {
        long val;
        char chars[long_size];
    } data;
    int i = 0;
    int j = len/long_size;
    while(i < j)
    {
        data.val = ptrace(PTRACE_PEEKTEXT, child, addr+i*8, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }

    j = len % long_size;
    if(j != 0)
    {
        data.val = ptrace(PTRACE_PEEKTEXT, child, addr+i *8, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len]='\n';
}


void putdata(pid_t child, long addr, char *str, int len)
{
    char *laddr = str;
    int i = 0;
    int j = len / long_size;
    union u
    {
        long val;
        char chars[long_size];
    } data;

    while(i < j)
    {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child, addr+i *8, data.val);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0)
    {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child, addr+i*8, data.val);
    }
}

int main(int argc, char* argv[])
{
    
    struct user_regs_struct regs, newregs;
    char code[] = {0xcd, 0x80,0xcc,0};
    char backup[4];
    if(argc, != 2)
    {
        printf("Usage: %s <pid to be traced>",argv[0], argv[1]);
        exit(1);
    }

    pid_t traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

    getdata(traced_process, regs.rip, backup, 3);
    putdata(traced_process, regs.rip, code, 3);

    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    wait(NULL);
    printf("The process stopped ,putting back\n Press <enter> to continue\n");
    getchar();
    putdata(traced_process, regs.rip, backup, 3);
    ptrace(PTRACE_SETREGS, traced_process,NULL, &regs);
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
    return 0;
}

