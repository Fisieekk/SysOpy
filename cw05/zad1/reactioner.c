// Zadanie 1
// Napisz program demonstrujący różne reakcje na przykładowy sygnał SIGUSR1 w zależności od ustawionych dyspozycji. 
// Reakcja na sygnał SIGUSR1 programu powinna zależeć od wartości argumentu z linii poleceń. 
// Argument ten może przyjmować wartości: none, ignore, handler, mask. 
// Program w zależności od parametru odpowiednio: nie zmienia reakcji na sygnał, ustawia ignorowanie, instaluje handler obsługujący sygnał (działający w ten sposób, że wypisuje komunikat o jego otrzymaniu), maskuje ten sygnał oraz sprawdza czy wiszący/oczekujący sygnał jest widoczny. 
// Następnie przy pomocy funkcji raise wysyła sygnał do samego siebie oraz wykonuje odpowiednie dla danej opcji, opisane wyżej działania.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void handler(int sig){
    printf("Signal %d received \n",sig);
}

int main(int argc, char **argv){
    if (argc != 2){
        printf("Wrong number of arguments\n");
        return 1;
    }

    if (strcmp(argv[1], "none") == 0){
        signal(SIGUSR1, SIG_DFL); 
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "mask") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);
        sigset_t pending_signals;
        sigpending(&pending_signals);
        printf("Is signal pending? : %i\n", sigismember(&pending_signals, SIGUSR1));
    }
    return 0;
}