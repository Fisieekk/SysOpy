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


// User.c
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



void generate_random_string(char *str, int len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    int i;
    for (i = 0; i < len; i++) {
        int index = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[index];
    }
    str[len] = '\0';
}


int main(int argc,char** argv){
    if(argc!=2){
        printf("Bad number of arguments\n");
        return 1;
    }
    long N = atoi(argv[1]);
    
    int shm_fd = shm_open("printer", O_RDWR, S_IRUSR | S_IWUSR);
    if(shm_fd == -1){
        perror("shm_open");
        return 1;
    }

    printer_array_t* printers = mmap(NULL, sizeof(printer_array_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(printers == MAP_FAILED){
        perror("mmap");
        return 1;
    }
    char user_buffer[MAX_TEXT_LEN] = {0};
    for(int i=0;i<N;i++){
        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            return 1;
        }
        else if(pid == 0){
            for(;;){
                generate_random_string(user_buffer, 10);

                int printer_index = -1;
                for (int j = 0; j < printers->printers_len; j++) {
                    int val;
                    sem_getvalue(&printers->printers[j].sem, &val);
                    if(val > 0) {
                        printer_index = j;
                        break;
                    }
                }
                if(printer_index == -1){
                    printer_index = rand() % printers->printers_len;
                }
                if(sem_wait(&printers->printers[printer_index].sem) == -1){
                    perror("sem_wait");
                    return 1;
                }
                memcpy(printers->printers[printer_index].text, user_buffer, 256);
                printers->printers[printer_index].text_len = strlen(user_buffer);

                printers->printers[printer_index].state = BUSY;
                
                printf("User %d sent text: %s\n", i, printers->printers[printer_index].text);
                fflush(stdout);
                sleep(rand()%4+2);
            }
            exit(0);
        }
    }

     while(wait(NULL) > 0) {};

    munmap(printers, sizeof(printer_array_t));
}