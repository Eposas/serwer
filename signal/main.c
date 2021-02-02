#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int count=0;
static void pSigHandler(int signo){
    switch (signo) {
        case SIGUSR1:
            write(1, "to mnie nie zabije\n", sizeof("to mnie nie zabije"));
            count++;
            return;
    }
}

int main(void)
{
    struct sigaction psa;
    psa.sa_handler = pSigHandler;
    sigaction(SIGUSR1, &psa, NULL);
    for(;;) pause();
    return 0;
}