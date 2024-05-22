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
volatile int status = -1;
volatile int requests_ammount = 0;

void actioner(int sig, siginfo_t *info, void *ucontext){
    printf("%d\n", info -> si_int);
    if (sig == SIGUSR1){
        pid_t sender_pid = info -> si_pid;
        printf("Signal %d received from %d\n",sig, sender_pid); 
        status = info -> si_int;
        requests_ammount++;
        kill(sender_pid, SIGUSR1);
    }
}
int main(){
    pid_t catcher_pid = getpid();
    printf("Catcher PID: %d\n", catcher_pid);
    struct sigaction act;
    act.sa_sigaction = actioner;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);  
    sigaction(SIGUSR1, &act, NULL);
    while(1){
        switch(status){
            case 1:
                for (int i = 1; i <= 100; i++){
                    printf("%d\n", i);
                    status=-1;
                }
                break;
            case 2:
                printf("Number of requests: %d\n", requests_ammount);
                status=-1;
                break;
            case 3:
                printf("Exiting\n");    
                exit(0);
            default:
                break;
        }
    }
    return 0;

}   