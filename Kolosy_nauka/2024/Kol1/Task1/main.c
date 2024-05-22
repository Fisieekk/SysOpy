#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char *argv[]){
   pid_t p;
   switch (p=fork()) {
    case -1:{
      perror("fork");
      exit(1);}
    case 0:{  //proces potomny
      sleep(3);
      exit(3);}
    default: {  //proces macierzysty
      int status;
      if  (argc>1){
       if (atoi(argv[1])==2){
        kill(p, SIGINT);
        printf("Wysłano sygnał SIGINT\n");
       }

       else if (atoi(argv[1])==9){
        kill(p, SIGKILL);
        printf("Wysłano sygnał SIGKILL\n");
       }

      else printf("Sygnał nieobsłużony\n");
      }
      else printf("Zakończenie przez EXIT\n");
// Czekaj na proces potomny i pobierz jego status zakończenia
//  1) Jeśli proces zakończył się przez exit, wypisz wartość statusu
//  2) Jeśli proces zakończył się sygnałem, wypisz numer sygnału
//  W obu przypadkach powinna zostać zwrócona informacja "Potomek o PID=xxx zakończył się kodem/sygnałem xxx

      wait(&status);
      if (WIFEXITED(status))
        printf("Potomek o PID=%d zakończył się kodem %d\n", p, WEXITSTATUS(status));
      else if (WIFSIGNALED(status))
        printf("Potomek o PID=%d zakończył się sygnałem %d\n", p, WTERMSIG(status));
      else
        printf("Potomek o PID=%d zakończył się nieoczekiwanie\n", p);




    }
   }
   return 0;
}
