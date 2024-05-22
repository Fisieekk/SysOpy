#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAX_MESSAGE_TEXT 2048
#define SERVER_QUEUE_NAME "/server"
#define CLIENT_QUEUE_NAME_SIZE 40
#define MAX_CLIENTS_COUNT 40
#define min(a, b) (a < b ? a : b)

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


mqd_t initialize_queue(const char* name, struct mq_attr* attr, int flags, mode_t mode) {
    mqd_t mqd = mq_open(name, flags, mode, attr);
    if (mqd < 0) perror("mq_open");
    return mqd;
}

void send_initial_message(mqd_t server_queue, const char* queue_name) {
    message_t message_init = {
            .type = INIT,
            .id = -1
    };
    memcpy(message_init.text, queue_name, min(CLIENT_QUEUE_NAME_SIZE - 1, strlen(queue_name)));
    if (mq_send(server_queue, (char*)&message_init, sizeof(message_init), 10) < 0) {
        perror("mq_send init");
    }
}

void handle_messages(mqd_t client_queue, int* pipe_write_end) {
    message_t message;
    while (1) {
        if (mq_receive(client_queue, (char*)&message, sizeof(message), NULL) >= 0) {
            switch (message.type) {
                case TEXT:
                    printf("Received from id: %d message: %s\n", message.id, message.text);
                    break;
                case ID:
                    printf("Received identifier from server: %d\n", message.id);
                    write(*pipe_write_end, &message.id, sizeof(message.id));
                    break;
                default:
                    printf("Unknown message type in client queue: %d", message.type);
                    break;
            }
        }
    }
}

void read_and_send_messages(mqd_t server_queue, int* pipe_read_end) {
    int id;
    read(*pipe_read_end, &id, sizeof(id));
    char* buffer = NULL;
    while (1) {
        if (scanf("%ms", &buffer) == 1) {
            message_t send_message = {
                    .type = TEXT,
                    .id = id
            };
            memcpy(send_message.text, buffer, min(MAX_MESSAGE_TEXT - 1, strlen(buffer)));
            mq_send(server_queue, (char*)&send_message, sizeof(send_message), 10);
            free(buffer);
            buffer = NULL;
        } else {
            perror("scanf input");
        }
    }
}

int create_pipes() {
    int parent_pipe[2];
    if (pipe(parent_pipe) < 0) {
        perror("pipe");
        return -1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork listener");
        return -1;
    } else if (pid == 0) { // Child
        close(parent_pipe[0]);
        return parent_pipe[1];  // Return the write end of the pipe
    } else { // Parent
        close(parent_pipe[1]);
        return parent_pipe[0];  // Return the read end of the pipe
    }
}


int main() {
    pid_t pid = getpid();
    char queue_name[CLIENT_QUEUE_NAME_SIZE] = {0};
    sprintf(queue_name, "/server: %d", pid);

    struct mq_attr attributes = {
            .mq_flags = 0,
            .mq_msgsize = sizeof(message_t),
            .mq_maxmsg = 10
    };

    mqd_t client_queue = initialize_queue(queue_name, &attributes, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    mqd_t server_queue = initialize_queue(SERVER_QUEUE_NAME, NULL, O_RDWR, S_IRUSR | S_IWUSR);

    send_initial_message(server_queue, queue_name);

    int pipe_end = create_pipes();
    if (pipe_end < 0) return 1;  // Error in pipe or fork

    if (getpid() == pid) { // Parent
        read_and_send_messages(server_queue, &pipe_end);
    } else { // Child
        handle_messages(client_queue, &pipe_end);
    }
}
