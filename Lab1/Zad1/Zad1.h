#define Zad1_h
#include <stdlib.h>

typedef struct Zad1Array
{
    char **zad1Array;
    size_t size;
    size_t max_size;
}Zad1Array;

Zad1Array init_array(int size);
void count_line_words(Zad1Array *array, char *filename);
char* get_elements_from_block(Zad1Array *array,int index);
void delete_block(Zad1Array *array,int index);
void free_array(Zad1Array *array);