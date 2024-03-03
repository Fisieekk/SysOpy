#include "Zad1.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 256
// 1
Zad1Array init_array(int size){
    Zad1Array array;
    array.zad1Array=(char**) calloc(size, sizeof(char*));
    array.size=0;
    array.max_size=size;
    
    return array;
}
// 2
void count_line_words(Zad1Array *array,char *filename){
    char cmd[SIZE];
    snprintf(cmd,SIZE-1,"wc %s > /tmp/count_file",filename);
    system(cmd);

    FILE *tmpfile =fopen("/tmp/count_file","r");
    if(tmpfile==NULL){
        printf("File doesn't exist");
        return;
    }
    char output[SIZE];
    if(fgets(output,SIZE,tmpfile)==NULL){
        printf("File empty");
        return;
    }
    fclose(tmpfile);

    if(array->size >=array->max_size){
        printf("Max size reached");
        return;
    }
    size_t n=strlen(output);
    array->zad1Array[array->size]=calloc(n,sizeof(char)*n);
    array->zad1Array[array->size]=strcpy(array->zad1Array[array->size],output);
    array->size++;

    system("rm -f /tmp/count_file");
}
// 3
char* get_elements_from_block(Zad1Array *array,int index){
    if(index >=array->max_size|| index >= array->size){
        return("Too big index");
    }
    return (array->zad1Array[index]);
}
// 4
void delete_block(Zad1Array *array,int index){
    free(array->zad1Array[index]);
}
// 5
void free_array(Zad1Array *array){
    for(int i=0;i<array->size;i++){
        free(array->zad1Array[i]);
    }
    free(array->zad1Array);
}