#include<stdio.h>
#include <sys/user.h>
int main(int argc, char *argv[])
{
    printf("Hello ,world\n");
    struct user_regs_struct regs;
    regs.eip = 1;
    return 0;
}
