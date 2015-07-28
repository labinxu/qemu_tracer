#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
const int long_size = sizeof(long);

void reverse(char *str)
{
    char temp;
    for(int i = 0, j = strlen(str)-2;
        i <= j;++i,--j)
    {
        temp = str[i];
        str[i] =  str[j];
        str[j] = temp;
    }
}

void get_data(pid_t child, long addr,
              char *str, int len)
{

    union u {
        long val;
        char chars[long_size];
    }data;

    int i = 0;
    int j = len/long_size;
    char *laddr = str;
    while(i<j)
    {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr+i*8,NULL);
        memcpy(laddr, data.chars,long_size);
        ++i;
        laddr += long_size;
        //std::cout<<"read "<<data.val<<std::endl;
    }
    j = len % long_size;
    if(j != 0)
    {
        data.val = ptrace(PTRACE_PEEKDATA,
                          child, addr+i*8,
                          NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len]='\0';
}

void put_data(pid_t child, long addr, char *str, int len)
{
    union u
    {
        long val;
        char chars[long_size];
    } data;
    int i = 0;
    int j = len/long_size;
    char* laddr = str;
    while(i < j)
    {
        memcpy(data.chars, laddr, long_size);
        ptrace(PTRACE_POKEDATA, child, addr + i*8, data.val);
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
int main()
{
    pid_t child = fork();
    if(child == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else
    {
        long orig_eax;
        long params[15];
        int status;
        char *str;
        int toggle = 0;
        std::cout<<"SYS_write "<<SYS_write<<" pid "<<child<<std::endl;
        while(true)
        {
            wait(&status);
            if(WIFEXITED(status))
            {
                break;
            }
            orig_eax = ptrace(PTRACE_PEEKUSER,
                              child, 8*ORIG_RAX,
                              NULL);

            if(orig_eax == SYS_write)
            {
                if(toggle ==0)
                {
                    toggle = 1;
                    //params[0] = ptrace(PTRACE_PEEKUSER,child, 4 * EBX,NULL);
                    //params[1] = ptrace(PTRACE_PEEKUSER,child, 4 * ECX,NULL);
                    //params[2] = ptrace(PTRACE_PEEKUSER,child, 4 * EDX,NULL);

                    params[0] = ptrace(PTRACE_PEEKUSER,child, 8 * RDI,NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER,child, 8 * RSI,NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER,child, 8 * RDX,NULL);
                    params[3] = ptrace(PTRACE_PEEKUSER,child, 8 * R10,NULL);
                    str = (char*)malloc((params[2]+1)*sizeof(char));
                    get_data(child, params[1], str, params[2]);
                    //std::cout<<"str "<<str<<std::endl;
                    //std::cout<<"params 0 : "<<(int)params[0]<<std::endl;
                    //std::cout<<"params 1 : "<<(int)params[1]<<std::endl;
                    //std::cout<<"params 2 : "<<(int)params[2]<<std::endl;
                    //std::cout<<"params 3 : "<<(int)params[3]<<std::endl;
                    reverse(str);
                    put_data(child,params[1],str, params[2]);
                }
                else
                {
                    toggle = 0;
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }

        return 0;
    }

}
