// Napisz dwa programy: sender program wysyłający sygnały SIGUSR1 i catcher - program odbierający te sygnały. Program catcher jest uruchamiany jako pierwszy, wypisuje swój numer PID i czeka na sygnały SIGUSR1. Po każdorazowym odebraniu sygnału SIGUSR1 przez program catcher powinno nastąpić potwierdzenie odbioru tego sygnału. W tym celu, catcher wysyła do sendera sygnał SIGUSR1 informujący o odbiorze sygnału. Sender powinien wysłać kolejny sygnał dopiero po uzyskaniu tego potwierdzenia. Czekanie na takie potwierdzenie może odbywac się wywołując funkcję sigsuspend. Wraz z każdym sygnałem przesłanym przez sender powinien zostać przesłany tryb pracy programu catcher za pomocą funkcji sigqueue. Możliwe tryby pracy:

// 1- wypisanie na standardowym wyjściu liczb od 1 do 100

// 2 - wypisanie na standardowym wyjściu liczby otrzymanych żądań zmiany trybu pracy od początku działania programu

// 3 – zakończenie działania programu catcher.

// PID sendera catcher pobiera ze struktury siginfo_t po przechwyceniu od niego sygnału. Program sender jako pierwszy parametr przyjmuje PID procesu catcher. Drugi parametr określa tryb pracy procesu catcher - w jednym wywołaniu może być przekazany jeden taki tryb. Program catcher działa aż do momentu otrzymania trybu 3 (zakończenie działania programu catcher). Program sender działa do momentu otrzymania potwierdzenia otrzymania przez catcher przesłanego trybu, po czym kończy pracę. Program sender można wywołać wielokrotnie aż do zakończenia pracy przez catcher.

// UWAGA! W żaden sposób nie opóźniamy wysyłania sygnałów, wszelkie "gubienie" sygnałów jest zjawiskiem naturalnym.


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


int main(int argc , char** argv){
    if (argc != 3){
        printf("Wrong number of arguments\n");
        return 1;
    }
    pid_t sender_pid = getpid();
    printf("Sender PID: %d\n", sender_pid);
    signal(SIGUSR1, handler);
    long catcher_pid = strtol(argv[1], NULL, 10);
    long mode = strtol(argv[2], NULL, 10);
    union sigval value = {mode};
    sigqueue(catcher_pid, SIGUSR1, value);
    printf("Signal sent with argument %ld\n", mode);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    sigsuspend(&mask);
    return 0;
}
