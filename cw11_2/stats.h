#include <time.h>
#include <arpa/inet.h>


#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 1024
#define MAX_NAME_LENGTH 32
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
    char args[MAX_COMMAND_ARGS][MAX_MESSAGE_LENGTH];
} message_t;