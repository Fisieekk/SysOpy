#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 2560
#define MAX_CLIENTS_QUEUE 10

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

void handle_init(message_t *message, mqd_t *client_queues,int *registeredClients){
    client_queues[*registeredClients] = mq_open(message->text, O_WRONLY);
    if(client_queues[*registeredClients] == -1){
        perror("mq_open");
        exit(1);
    }

    message_t response;
    response.id = *registeredClients;
    response.type = ID;
    mq_send (client_queues[*registeredClients], (char *)&response, sizeof(message_t), 0);
    *registeredClients += 1;
}

void handle_message(mqd_t *clientQueues,message_t *message,int registeredClients){
    for(int i = 0; i < registeredClients; i++){
        if(i != message->id){
            mq_send(clientQueues[i], (char *)message, sizeof(message_t), 0);
        }
    }
}

mqd_t create_queue(){
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(message_t);
    return mq_open("/server",  O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attr);
    
}

int main(){
    mqd_t server_queue = create_queue();
    if (server_queue == -1){
        perror("mq_open");
        exit(1);
    }

    mqd_t client_queues[MAX_CLIENTS_QUEUE];
    int registeredClients = 0;

    while(1){
        message_t message;
        if(mq_receive(server_queue, (char *)&message, sizeof(message_t), NULL) == -1){
            perror("mq_receive");
            exit(1);
        }
        printf("Received message of type %s\n", message.type == INIT ? "INIT" : "TEXT");
        switch (message.type){
            case INIT:
                handle_init(&message, client_queues, &registeredClients);
                break;
            case TEXT:
                handle_message(client_queues, &message, registeredClients);
                break;
            default:
                printf("Unknown message type\n");
                break;
        }
    }
}