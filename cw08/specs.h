#ifndef SPECS_H
#define SPESPEC_H
#include <semaphore.h>


#define MAX_PRINTERS 10
#define MAX_TEXT_LEN 100

typedef enum {
    FREE,
    BUSY
} state_t;


typedef struct {
    sem_t sem;
    char text[MAX_TEXT_LEN];
    size_t text_len;
    state_t state;
} printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    size_t printers_len;
} printer_array_t;

#endif