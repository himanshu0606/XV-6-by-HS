#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// here we are creating a user space such that user can call the syscalls himself
int main(int argc, char *argv[])
{
    char *HA_argv[100];
    if(argc < 3)
    {
        fprintf(2,"used: %s mask command\n", argv[0]);
        exit(1);
    }

    if(trace(atoi(argv[1]))<0)
    {
        fprintf(2,"%s: strace failed\n", argv[0]);
        exit(1);
    }
    for(int i=0;i<argc && i<MAXARG;i++)
    {
        HA_argv[i-2] = argv[i];
    }

    exec(HA_argv[0],HA_argv);

    exit(0);
}