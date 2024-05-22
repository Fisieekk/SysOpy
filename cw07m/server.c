#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_MESSAGE_TEXT 2048
#define SERVER_QUEUE_NAME "/server"
#define MAX_CLIENTS_COUNT 100

typedef enum {
    INIT,
    ID,
    TEXT
} message_type_t;

typedef struct {
    message_type_t type;
    int id;
    char text[MAX_MESSAGE_TEXT];
} message_t;
mqd_t create_queue() {
    struct mq_attr attributes = {
            .mq_flags = 0,
            .mq_msgsize = sizeof(message_t),
            .mq_maxmsg = 10
    };

    return mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attributes);
}

mqd_t openQueue(const char *queueName) {
    return mq_open(queueName, O_RDWR, S_IRUSR | S_IWUSR, NULL);
}

void handle_init(message_t *message, mqd_t *clientQueues, int *registeredClients) {
    printf("Received INIT message: %s\n", message->text);
    clientQueues[*registeredClients] = openQueue(message->text);
    if(clientQueues[*registeredClients] < 0) {
        perror("mq_open client");
        return;
    }

    message_t response = {
            .type = ID,
            .id = *registeredClients
    };

    mq_send(clientQueues[*registeredClients], (char*)&response, sizeof(response), 10);
    (*registeredClients)++;
}

void handle_message(mqd_t *clientQueues, message_t *message, int registeredClients) {
    printf("Received TEXT message from id: %d\n", message->id);
    for (int identifier = 0; identifier < registeredClients; identifier++) {
        if (identifier != message->id) {
            mq_send(clientQueues[identifier], (char*)message, sizeof(message_t), 10);
        }
    }
}



int main() {
    mqd_t serverQueue = create_queue();
    if (serverQueue < 0) {
        perror("mq_open server");
        return 1;
    }

    mqd_t clientQueues[MAX_CLIENTS_COUNT];
    int registeredClients = 0;

    printf("Server started...\n");

    while(1) {
        message_t receiveMessage;
        if (mq_receive(serverQueue, (char*)&receiveMessage, sizeof(receiveMessage), NULL) == -1) {
            perror("mq_receive");
            continue;
        }

        switch(receiveMessage.type) {
            case INIT:
                handle_init(&receiveMessage, clientQueues, &registeredClients);
                break;
            case TEXT:
                handle_message(clientQueues, &receiveMessage, registeredClients);
                break;
            default:
                printf("Unexpected message type in server queue: %d\n", receiveMessage.type);
                break;
        }
    }

    return 0;
}
