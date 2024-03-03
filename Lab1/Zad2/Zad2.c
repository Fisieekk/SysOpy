#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#ifndef DYNAMIC
#include "Zad1.h"
#else
#include <dlfcn.h>
typedef struct Zad1Array
{
    char **zad1Array;
    size_t size;
    size_t max_size;
}Zad1Array;

#endif
#define SIZE 256

int main(int args,char **argv){
    #ifndef DYNAMIC
    void *handler =dlopen("./libcount_dynamic.so",RTLD_LAZY);
    if(!handler){
        printf("Failed to dlopen");
        return;
    }
    Zad1Array (*init_array)(int);
    void (*count_line_words)(Zad1Array *, char *);
    char* (*get_elements_from_block)(Zad1Array *, int);
    void (*delete_block)(Zad1Array *, int);
    void (*free_array)(Zad1Array *);

    init_array=(Zad1Array (*)(int)) dlsym(handler,init_array);
    count_line_words=(void (*)(Zad1Array*, char*)) dlsym(handler,count_line_words);
    get_elements_from_block=(char* (*)(Zad1Array*,int)) dlsym(handler,get_elements_from_block);
    delete_block=(void (*)(Zad1Array*, int)) dlsym(handler,delete_block);
    free_array=(void (*)(Zad1Array *)) dlsym(handler,free_array);
    #endif

    char buffer[SIZE];
    char cmd[SIZE];
    Zad1Array array;

    while(strcmp(cmd,"exit")!=0){
        fgets(buffer,SIZE,stdin);
        sscanf(buffer,"%s",cmd);

        struct timespec start_ts,end_ts;
        clock.gettime(CLOCK_REALTIME,&start_ts);
    }
}
