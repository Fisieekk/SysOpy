// Napisz program, który przyjmuje jeden argument: argv[1]. 
// Program ma utworzyć argv[1] procesów potomnych. 
// Każdy proces potomny ma wypisać na standardowym wyjściu w jednym wierszu dwa identyfikatory: identyfikator procesu macierzystego i swój własny. 
// Na końcu standardowego wyjścia proces macierzysty ma wypisać argv[1]. 
// Wskazówka: aby program na pewno wypisywał argv[1] jako ostatni wiersz standardowego wyjścia, należy użyć funkcji systemowej wait().


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("%d\n", argc);
    if (argc != 2) {
        printf("Usage: %s <number_of_processes>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("Parent: %d, Child: %d\n", getppid(), getpid());
            return 0;
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("%s\n", argv[1]);
    return 0;
}