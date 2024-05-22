// Napisz prosty program typu klient-serwer, w którym komunikacja zrealizowana jest za pomocą kolejek komunikatów.
// Serwer po uruchomieniu tworzy nową kolejkę komunikatów. Za pomocą tej kolejki klienci będą wysyłać komunikaty do serwera. Komunikaty wysyłane przez klientów mogą zawierać polecenie oznaczające pierwsze nawiązanie połaczenia z serwerem (INIT) lub jeśli wcześniej połączenie zostało już nawiązane: identyfikator klienta wraz z komunikatem, który ma zostać przekazany przez serwer do wszystkich pozostałych klientów. W odpowiedzi na polecenie INIT, serwer ma przesłać identyfikator nadany nowemu klientowi.
// Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC i wysyła jej klucz do serwera wraz z komunikatem INIT. Po otrzymaniu takiego komunikatu, serwer otwiera kolejkę klienta, przydziela klientowi identyfikator (np. numer w kolejności zgłoszeń) i odsyła ten identyfikator do klienta (komunikacja w kierunku serwer->klient odbywa się za pomocą kolejki klienta). Po otrzymaniu identyfikatora, klient może wysłać do serwera komunikaty, które serwer będzie przesyłał do wszystkich pozostałych klientów. Komunikaty te są czytane ze standardowego wejścia. Klient po uruchomieniu tworzy drugi proces, który powinien odbierać komunikaty wysyłane przez serwer (przy użyciu kolejki komunikatów klienta) i wyświetlać te komunikaty na standardowym wyjściu.

// Klient i serwer należy napisać w postaci osobnych programów. Serwer musi być w stanie pracować jednocześnie z wieloma klientami. Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbę klientów, którzy mogą się zgłosić do serwera).

// Powyższe zadanie można zrealizować wykorzystując mechanizmy POSIX.


// Klient

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CLIENTS 100
#define MAX_MSG_SIZE 256
#define MAX_CLIENTS_QUEUE 10
#define CLIENT_QUEUE_NAME_SIZE 40


#define min(a, b) (a < b ? a : b)


typedef enum{
    INIT,
    ID,
    TEXT,
} message_type_t;

typedef struct { 
    int id;
    message_type_t type;
    char text[MAX_MSG_SIZE];
} message_t ;

void handle_message(mqd_t client_queue,int *parent_pipe){
    message_t message;
    while(1){
        if(mq_receive(client_queue, (char *)&message, sizeof(message_t), NULL) >=0){
            switch (message.type){
            case TEXT:
                printf("Received message: %s\n",message.text);
                exit(0);
            case ID:
                printf("Received id: %d\n",message.id);
                write(*parent_pipe,&message.id,sizeof(int));
                exit(0);
            default:
                printf("Received unknown message type\n");
                break;
            }
        }
    }
}

void reader(mqd_t server_queue , int *parent_pipe){
    int id;
    read(*parent_pipe,&id,sizeof(int));
    char* buff = NULL;
    while(1){
        // struct mq_attr atrr;
        // mq_getattr(server_queue, &atrr);
        // if(atrr.mq_curmsgs >= atrr.mq_maxmsg) {
        //     printf("Server is busy, please wait\n");
        //     continue;
        // }
        if (scanf("%ms",&buff) == 1){
            message_t message;
            message.id = id;
            message.type = TEXT;
            memcpy(message.text,buff,strlen(buff)+1);
            mq_send(server_queue, (char *)&message, sizeof(message_t), 0);
            free(buff);
            buff = NULL;
        }
        else{
            perror("scanf");
        }
    }
}

int create_pipes(){
    int to_parent_pipe[2];
    if (pipe(to_parent_pipe) < 0) {
        perror("pipe");
        return -1;
    }
    pid_t pid = fork();
    printf("pid: %d\n",pid);
    if (pid < 0) {
        perror("fork listener");
        return -1;
    } else if (pid == 0) { // Child
        printf("child");
        close(to_parent_pipe[0]);
        return to_parent_pipe[1];  // Return the write end of the pipe
    } else { // Parent
        printf("parent");
        close(to_parent_pipe[1]);
        return to_parent_pipe[0];  // Return the read end of the pipe
    }
}



int main(){
    pid_t pid = getpid();
    char client_queue_name[64]={0};
    sprintf(client_queue_name,"/%d",pid);

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_CLIENTS_QUEUE;
    attr.mq_msgsize = sizeof(message_t);

    mqd_t client_queue = mq_open(client_queue_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR,&attr);
    if(client_queue == -1){
        perror("mq_open");
        exit(1);
    }

    mqd_t server_queue = mq_open("/server",O_RDWR, S_IRUSR | S_IWUSR);
    if(server_queue == -1){
        perror("mq_open");
        exit(1);
    }

    message_t message;
    message.type = INIT;
    message.id = -1;

    // memcpy(message.text,client_queue_name,strlen(client_queue_name)+1);
    memcpy(message.text, client_queue_name, min(CLIENT_QUEUE_NAME_SIZE - 1, strlen(client_queue_name)));

    if(mq_send(server_queue, (char *)&message, sizeof(message_t), 10) == -1){
        perror("mq_send");
        exit(1);
    }
    int parent_pipe = create_pipes();
    if(parent_pipe == -1){
        exit(1);
    }
    if(getpid() == pid){
        reader(server_queue,&parent_pipe);
    }
    else{
        handle_message(client_queue,&parent_pipe);
    }

    
}
