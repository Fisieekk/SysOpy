// Napisz prosty program typu klient-serwer, w którym komunikacja zrealizowana jest za pomocą kolejek komunikatów.
// Serwer po uruchomieniu tworzy nową kolejkę komunikatów. Za pomocą tej kolejki klienci będą wysyłać komunikaty do serwera. Komunikaty wysyłane przez klientów mogą zawierać polecenie oznaczające pierwsze nawiązanie połaczenia z serwerem (INIT) lub jeśli wcześniej połączenie zostało już nawiązane: identyfikator klienta wraz z komunikatem, który ma zostać przekazany przez serwer do wszystkich pozostałych klientów. W odpowiedzi na polecenie INIT, serwer ma przesłać identyfikator nadany nowemu klientowi.
// Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC i wysyła jej klucz do serwera wraz z komunikatem INIT. Po otrzymaniu takiego komunikatu, serwer otwiera kolejkę klienta, przydziela klientowi identyfikator (np. numer w kolejności zgłoszeń) i odsyła ten identyfikator do klienta (komunikacja w kierunku serwer->klient odbywa się za pomocą kolejki klienta). Po otrzymaniu identyfikatora, klient może wysłać do serwera komunikaty, które serwer będzie przesyłał do wszystkich pozostałych klientów. Komunikaty te są czytane ze standardowego wejścia. Klient po uruchomieniu tworzy drugi proces, który powinien odbierać komunikaty wysyłane przez serwer (przy użyciu kolejki komunikatów klienta) i wyświetlać te komunikaty na standardowym wyjściu.

// Klient i serwer należy napisać w postaci osobnych programów. Serwer musi być w stanie pracować jednocześnie z wieloma klientami. Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbę klientów, którzy mogą się zgłosić do serwera).

// Powyższe zadanie trzeba zrealizować wykorzystując mechanizmy System V.

// Serwer


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
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define MAX_CLIENTS_QUEUE 10

typedef struct msgbuf {
    long mtype;
    char mtext[MAX_MSG_SIZE];
} msgbuf;

typedef struct client {
    int id;
    int queue;
} client;

int clients_count = 0;
client clients[MAX_CLIENTS];
int server_queue;
int clients_queues[MAX_CLIENTS_QUEUE];
int clients_queues_count = 0;

void handle_sigint(int sig) {
    for (int i = 0; i < clients_queues_count; i++) {
        msgctl(clients_queues[i], IPC_RMID, NULL);
    }
    msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

int get_client_queue(int id) {
    for (int i = 0; i < clients_count; i++) {
        if (clients[i].id == id) {
            return clients[i].queue;
        }
    }
    return -1;
}

void send_message(int queue, char *message) {
    msgbuf msg;
    msg.mtype = 1;
    strcpy(msg.mtext, message);
    if (msgsnd(queue, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
}

void send_message_to_all_clients(char *message) {
    for (int i = 0; i < clients_count; i++) {
        send_message(clients[i].queue, message);
    }
}

void handle_init_message(int client_queue) {
    if (clients_count == MAX_CLIENTS) {
        send_message(client_queue, "Server is full");
        return;
    }
        clients[clients_count].id = clients_count + 1;
    clients[clients_count].queue = client_queue;
    char message[MAX_MSG_SIZE];
    sprintf(message, "Your id is %d", clients_count + 1);
    send_message(client_queue, message);
    clients_count++;
}

void handle_client_message(int client_id, char *message) {
    char message_to_send[MAX_MSG_SIZE];
    sprintf(message_to_send, "Client %d: %s", client_id, message);
    send_message_to_all_clients(message_to_send);
}

void handle_message(msgbuf *msg) {
    char *message = msg->mtext;
    int client_id = msg->mtype;
    if (client_id == 1) {
        handle_init_message(msg->mtype);
    } else {
        handle_client_message(client_id, message);
    }
}

int main(){
    key_t server_key = ftok("/tmp", 0);
    if (server_key == -1) {
        perror("ftok");
        exit(1);
    }

    server_queue = msgget(server_key, IPC_CREAT | 0666);
    if (server_queue == -1) {
        perror("msgget");
        exit(1);
    }

    signal(SIGINT, handle_sigint);

    msgbuf msg;
    while (true) {
        if (msgrcv(server_queue, &msg, sizeof(msg.mtext), 0, 0) == -1) {
            if (errno == EINTR) {
                handle_sigint(0);
            }
            perror("msgrcv");
            exit(1);
        }
        handle_message(&msg);
    }
    return 0;   
}
