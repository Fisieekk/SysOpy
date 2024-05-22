// Wykorzystując semafory i pamięć wspólną ze standardu POSIX napisz program symulujący działanie systemu wydruku: 

// System składa się z N użytkowników oraz M drukarek. 
// Każdy z użytkowników może wysłać do systemu zadanie wydruku tekstu składającego się z 10 znaków. 
// Drukarka, która nie jest aktualnie zajęta, podejmuje się zadania "wydruku" tekstu danego zadania. 
// Wydruk w zadaniu polega na wypisaniu na standardowym wyjściu znaków wysłanych wcześniej do wydruku w ten sposób, że każdy następny znak wpisywany jest co jedną sekundę.
// Jeżeli wszystkie drukarki są zajęte to zlecenia wydruku są kolejkowane w opisywanym systemie wydruku. 
// Jeżeli kolejka jest pełna to użytkownik chcący zlecić zadanie wydruku czeka do momentu gdy będzie można zlecenie wpisać do kolejki.

// Każdy z N użytkowników powinien przesyłać do systemu wydruku zadanie wydruku 10 losowych znaków (od 'a' do 'z') 
// a następnie odczekać losową liczbe sekund. Zadania zlecenia wydruku i odczekania powinny być wykonane w nieskończonej pętli. 

// Należy zsynchronizować prace użytkowników oraz drukarek. Należy użyć mechanizmów POSIX.


// Printer.c
// Program symulujący działanie systemu wydruku

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include "specs.h"



int main(int argc,char** argv){
    if (argc != 2) {
        printf("Bad number of arguments\n");
        return 1;
    }

    long M = atoi(argv[1]);

    if (M < 1) {
        printf("Number of printers must be greater than 0\n");
        return 1;
    }
    if (M > 10) {
        printf("Number of printers must be less than 10\n");
        return 1;
    }

    int shm_fd = shm_open("printer", O_RDWR | O_CREAT ,  S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, sizeof(printer_array_t)) == -1) {
        perror("ftruncate");
        return 1;
    }

    printer_array_t* printers = mmap(NULL, sizeof(printer_array_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (printers == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    memset(printers->printers, 0, sizeof(printer_array_t));
    printers->printers_len = M;
    for (int i = 0; i < M; i++) {
        sem_init(&printers->printers[i].sem, 1, 1);
        pid_t pid = fork();
        if(pid==-1){
            perror("fork");
            return 1;
        }
        else if(pid==0){
            for(;;){
                if(printers->printers[i].state == BUSY){
                    printf("Printer %d is busy\n", i);
                    printf("Printer %d: ", i);
                    for(int j=0;j<printers->printers[i].text_len;j++){
                        printf("%c", printers->printers[i].text[j]);
                        sleep(1);
                    }
                    printf("\n");
                    fflush(stdout);
                    printers->printers[i].state = FREE;
                    sem_post(&printers->printers[i].sem);
                }
            }
            exit(0);
        }
    }
    while(wait(NULL)>0){}
    for(int i=0;i<M;i++){
        sem_destroy(&printers->printers[i].sem);
    }
    munmap(printers, sizeof(printer_array_t));
    shm_unlink("printer");
    return 0;

}