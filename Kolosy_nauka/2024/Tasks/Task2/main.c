#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <error.h>


void sighandler(int signum, siginfo_t *info, void *context) {
    printf("Received signal: %d\n", signum);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;

    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
        sigfillset(&action.sa_mask);
        sigdelset(&action.sa_mask, SIGUSR1);
        sigaction(SIGUSR1, &action, NULL);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        union sigval value;
        value.sival_int = atoi(argv[1]);
        sigqueue(child, atoi(argv[2]), value);
    }

    return 0;
}
