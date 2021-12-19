#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

#include "AddrConfilm.hpp"

static struct sigaction orig_act;
static sigjmp_buf env;

static void signal_handler(int sig, siginfo_t *sig_info, void *sig_data)
{
    siglongjmp(env, 1);
}

int ValidateAddr(void *addr)
{
    int is_valid = false;
    struct sigaction newAct;

    sigemptyset(&newAct.sa_mask);
    sigaddset(&newAct.sa_mask, SIGSEGV);

    newAct.sa_flags = 0;
    newAct.sa_sigaction = signal_handler;
    sigaction(SIGSEGV, &newAct, &orig_act);
    if (sigsetjmp(env, true) == 0)
    {
        /* touch */
        volatile char c;
        printf("Readable check ->");
        c = *((char *)addr); /* read */
        printf("succeeded\n");
        printf("writeable check ->");
        *((char *)addr) = c; /* write */
        printf("succeeded\n");
        is_valid = true;
    }
    else
    {
        printf("failed\n");
        is_valid = false;
    }
    sigaction(SIGSEGV, &orig_act, NULL);
    return is_valid;
}
