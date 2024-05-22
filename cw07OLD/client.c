#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#define MAX_MSG_SIZE 2560

typedef struct msgbuf {
    long mtype;
    char mtext[MAX_MSG_SIZE];
} msgbuf;

int server_queue;
int client_queue;
int client_id;

void handle_sigint(int sig) {
    msgctl(client_queue, IPC_RMID, NULL);
    exit(0);
}

void send_message(int queue, char *message) {
    msgbuf msg;
    msg.mtype = client_id;
    strcpy(msg.mtext, message);
    if(msgsnd(queue, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
}

void receive_message() {
    msgbuf msg;
    while (true) {
        if (msgrcv(client_queue, &msg, sizeof(msg.mtext), 0, 0) == -1) {
            if (errno == EINTR) {
                handle_sigint(0);
            }
            perror("msgrcv");
            exit(1);
        }
        printf("%s\n", msg.mtext);
    }
}

int main(){
    key_t server_key = ftok("/tmp", 0);
    if (server_key == -1) {
        perror("ftok");
        exit(1);
    }
    printf("first part\n");
    printf("server_key: %d\n", server_key);
    server_queue = msgget(server_key, 0);
    if (server_queue == -1) {
        perror("msgget");
        printf("Wartość errno: %d\n", errno); // Wyświetlenie wartości errno
        exit(1);
    }
    printf("second part\n");
    key_t client_key = ftok("client.c", getpid());
    if (client_key == -1) {
        perror("ftok");
        exit(1);
    }
    printf("third part\n");
    client_queue = msgget(client_key, IPC_CREAT | 0666);
    if (client_queue == -1) {
        perror("msgget");
        exit(1);
    }
    printf("fourth part\n");
    signal(SIGINT, handle_sigint);
    printf("fifth part\n");
    char message[MAX_MSG_SIZE];
    sprintf(message, "INIT");
    send_message(server_queue, message);
    printf("sixth part\n");
    msgbuf msg;
    if (msgrcv(client_queue, &msg, sizeof(msg.mtext), 0, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("%s\n", msg.mtext);
    sscanf(msg.mtext, "Your id is %d", &client_id);
    printf("seventh part\n");
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    printf("eighth part\n");

    if (pid == 0) {
        receive_message();
    } else {
        while (true) {
            printf("Enter message: ");
            fgets(message, MAX_MSG_SIZE, stdin);
            send_message(server_queue, message);
        }
    }
    return 0;
}
