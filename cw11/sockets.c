// Napisz prosty chat typu klient-serwer w którym komunikacja pomiędzy uczestmnikami czatu / klientami / klientami i serwerem realizoiwana jest za pośrednictwem socketów z życiem protokołu strumieniowego.

// Adres / port serwera podawany jest jako argument jego uruchomienia

// Klient przyjmuje jako swoje argumenty:

// - swoją nazwę/identyfikator (string o z góry ograniczonej długości)
// - adres serwera (adres IPv4 i numer portu) 

// Protokół komunikacyjny powinien obsługiwać następujące operacje:

// LIST:
// Pobranie z serwera i wylistowanie wszystkich aktywnych klientów
// 2ALL string:
// Wysłania wiadomości do wszystkich pozostałych klientów. Klient wysyła ciąg znaków do serwera, a serwer rozsyła ten ciąg wraz z identyfikatorem nadawcy oraz aktualną datą do wszystkich pozostałych klientów
// 2ONE id_klienta string:
// Wysłanie wiadomości do konkretnego klienta. Klient wysyła do serwera ciąg znaków podając jako adresata konkretnego klienta o identyfikatorze z listy aktywnych klientów. Serwer wysyła ten ciąg wraz z identyfikatorem klienta-nadawcy oraz aktualną datą do wskazanego klienta.
// STOP: Zgłoszenie zakończenia pracy klienta.  Powinno skutkować usunięciem klienta z listy klientów przechowywanej na serwerze 
// ALIVE - serwer powinien cyklicznie "pingować" zarejestrowanych klientów, aby zweryfikować że wciąż odpowiadają na żądania, a jeśli nie - usuwać ich z listy klientów.
// Klient przy wyłączeniu Ctrl+C powinien wyrejestrować się z serwera
// Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbę klientów, którzy mogą jednocześnie byc uczestnikami czatu).



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 256
#define MAX_NAME_LENGTH 32
#define MAX_COMMAND_ARG_LENGTH 32
#define MAX_COMMAND_ARGS 3

typedef enum {
    LIST,
    STOP,
    ALIVE,
    _2ALL,
    _2ONE,
    UNKNOWN
} command_t;

typedef struct {
    int socket;
    char name[MAX_NAME_LENGTH];
    struct sockaddr_in address;
    time_t last_seen;
} client_t;

typedef struct {
    command_t command;
    char args[MAX_COMMAND_ARGS][MAX_COMMAND_ARG_LENGTH];
} message_t;

client_t clients[MAX_CLIENTS];
int clients_count = 0;
int server_socket;
pthread_t alive_thread;

void *alive(void *arg) {
    while (1) {
        for (int i = 0; i < clients_count; i++) {
            if (time(NULL) - clients[i].last_seen > 10) {
                printf("Client %s is not responding\n", clients[i].name);
                for (int j = i; j < clients_count - 1; j++) {
                    clients[j] = clients[j + 1];
                }
                clients_count--;
            }
        }
        sleep(1);
    }
}

void send_message(int socket, char *message) {
    if (send(socket, message, strlen(message), 0) == -1) {
        perror("send");
        exit(1);
    }
}

void send_message_to_all(char *message) {
    for (int i = 0; i < clients_count; i++) {
        send_message(clients[i].socket, message);
    }
}

void send_message_to_one(char *name, char *message) {
    for (int i = 0; i < clients_count; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            send_message(clients[i].socket, message);
            return;
        }
    }
}

void handle_message(int socket, message_t *message) {
    switch (message->command) {
        case LIST: {
            char response[MAX_MESSAGE_LENGTH];
            sprintf(response, "Active clients:\n");
            for (int i = 0; i < clients_count; i++) {
                sprintf(response + strlen(response), "%s\n", clients[i].name);
            }
            send_message(socket, response);
            break;
        }
        case STOP: {
            for (int i = 0; i < clients_count; i++) {
                if (clients[i].socket == socket) {
                    printf("Client %s disconnected\n", clients[i].name);
                    for (int j = i; j < clients_count - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    clients_count--;
                    break;
                }
            }
            break;
        }
        case ALIVE: {
            for (int i = 0; i < clients_count; i++) {
                if (clients[i].socket == socket) {
                    clients[i].last_seen = time(NULL);
                    break;
                }
            }
            break;
        }
        case _2ALL: {
            char response[MAX_MESSAGE_LENGTH];
            sprintf(response, "%s: %s", message->args[0], message->args[1]);
            send_message_to_all(response);
            break;
        }
        case _2ONE: {
            char response[MAX_MESSAGE_LENGTH];
            sprintf(response, "%s: %s", message->args[0], message->args[2]);
            send_message_to_one(message->args[1], response);
            break;
        }
        case UNKNOWN: {
            send_message(socket, "Unknown command\n");
            break;
        }
    }
}

void *handle_client(void *arg) {
    int client_socket = *(int *) arg;
    char buffer[MAX_MESSAGE_LENGTH];
    message_t message;
    while (1) {
        int length = recv(client_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        if (length == -1) {
            perror("recv");
            exit(1);
        } else if (length == 0) {
            break;
        }
        buffer[length] = '\0';
        printf("Received message: %s", buffer);
        char *token = strtok(buffer, " ");
        if (strcmp(token, "LIST") == 0) {
            message.command = LIST;
        } else if (strcmp(token, "STOP") == 0) {
            message.command = STOP;
        } else if (strcmp(token, "ALIVE") == 0) {
            message.command = ALIVE;
        } else if (strcmp(token, "2ALL") == 0) {
            message.command = _2ALL;
            strcpy(message.args[0], clients[0].name);
            strcpy(message.args[1], buffer + 5);
        } else if (strcmp(token, "2ONE") == 0) {
            message.command = _2ONE;
            strcpy(message.args[0], clients[0].name);
            token = strtok(NULL, " ");
            strcpy(message.args[1], token);
            strcpy(message.args[2], buffer + 6 + strlen(message.args[1]));
        } else {
            message.command = UNKNOWN;
        }
        handle_message(client_socket, &message);
    }
    close(client_socket);
    pthread_exit(NULL);
}

void handle_sigint(int sig) {
    for (int i = 0; i < clients_count; i++) {
        close(clients[i].socket);
    }
    close(server_socket);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) == -1) {
        perror("inet_pton");
        exit(1);
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(server_socket, 10) == -1) {
        perror("listen");
        exit(1);
    }
    signal(SIGINT, handle_sigint);
    pthread_create(&alive_thread, NULL, alive, NULL);
    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length);
        if (client_socket == -1) {
            if (errno == EINTR) {
                break;
            }
            perror("accept");
            exit(1);
        }
        char name[MAX_NAME_LENGTH];
        int length = recv(client_socket, name, MAX_NAME_LENGTH, 0);
        if (length == -1) {
            perror("recv");
            exit(1);
        }
        name[length] = '\0';
        printf("Client %s connected\n", name);
        clients[clients_count].socket = client_socket;
        strcpy(clients[clients_count].name, name);
        clients[clients_count].address = client_address;
        clients[clients_count].last_seen = time(NULL);
        clients_count++;
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &client_socket);
    }
    pthread_join(alive_thread, NULL);
    return 0;
}

