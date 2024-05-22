// Napisz program, który przyjmuje jeden argument: argv[1] — ścieżkę katalogu. 
// Program powinien wypisać na standardowym wyjściu swoją nazwę, korzystając z funkcji printf(). 
// Zadeklaruj zmienną globalną global, a następnie zmienną lokalną local. 
// W zależności od zwróconej wartości przez fork() dokonaj obsługi błędu, wykonaj proces rodzica / proces potomny. W procesie potomnym:
// wyświetl komunikat „child process”,
// dokonaj inkrementacji zmiennych global i local,
// wyświetl komunikat „child pid = %d, parent pid = %d”
// wyświetl komunikat „child's local = %d, child's global = %d”
// wykonaj program /bin/ls z argumentem argv[1], korzystając z funkcji execl(), zwracając przy tym jej kod błędu.
// W procesie rodzica:
// wyświetl komunikat „parent process”
// wyświetl komunikat “parent pid = %d, child pid = %d”
// wyświetl komunikat “Child exit code: %d”
// wyświetl komunikat “Parent's local = %d, parent's global = %d”
// zwróć stosowny kod błędu.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int global = 0;


int main(int argc,char** argv){
    if(argc != 2){
        printf("Wrong number of arguments\n");
        return 1;
    }
    int local = 0;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "ls", "-l", argv[1], NULL);
        perror("execl");
        return 1;
    }
    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n", getpid(), pid);
    int status=0;
    wait(&status);
    int child_exit_status= WEXITSTATUS(status);
    if (child_exit_status) {
        printf("Child exit code: %d\n", child_exit_status);
    }
    printf("Parent's local = %d, parent's global = %d\n", local, global);
    return child_exit_status;

}











// int main(){
//     int global = 0;
//     int local = 0;
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         return 1;
//     }
//     if (pid == 0) {
//         printf("child process\n");
//         global++;
//         local++;
//         printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
//         printf("child's local = %d, child's global = %d\n", local, global);
//         execl("/bin/ls", "ls", "-l", NULL);
//         perror("execl");
//         return 1;
//     }
//     printf("parent process\n");
//     printf("parent pid = %d, child pid = %d\n", getpid(), pid);
//     int status;
//     wait(&status);
//     if (WIFEXITED(status)) {
//         printf("Child exit code: %d\n", WEXITSTATUS(status));
//     }
//     printf("Parent's local = %d, parent's global = %d\n", local, global);
//     return 0;
// }