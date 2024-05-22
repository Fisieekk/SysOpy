#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_SIZE 1024
void single_byt(FILE *in, FILE *out, long size){
    char c;
    for (long i = size - 1; i >= 0; i--) {
        fseek(in, i, SEEK_SET);
        c = fgetc(in); 
        fputc(c, out); 
    }
}


void thousand_byte(FILE *in, FILE *out, long size){
    long size_copy=size;
    char buf[BUFF_SIZE];
    char c;
    while(size_copy>=BUFF_SIZE){
        size_copy-=BUFF_SIZE;
        fseek(in, size_copy, SEEK_SET);
        fread(buf, sizeof(char), BUFF_SIZE, in);
        for (long i = 0; i<BUFF_SIZE /2 ; i++) {
            c = buf[i];
            buf[i] = buf[BUFF_SIZE - i - 1];
            buf[BUFF_SIZE - i - 1] = c;
        }
        fwrite(buf, sizeof(char), BUFF_SIZE, out);
    }
}


int main(int path, char *paths[]) {
    FILE *in;
    FILE *out;
    FILE *time;
    __clock_t start, end;
    double cpu_time_used;
    in = fopen(paths[1], "r");
    out = fopen(paths[2], "w");

    if (in == NULL || out == NULL) {
        printf("Error opening files.\n");
        exit(1);
    }
    time = fopen("time.txt", "a");
    if(time == NULL){
        printf("Error opening time.txt\n");
        exit(1);
    }

    fseek(in, 0, SEEK_END);
    long fileSize = ftell(in);
    fseek(in, -1, SEEK_CUR);
    #ifdef SINGLE
        printf("Single byte\n");
        start = clock();
        single_byt(in, out, fileSize);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(time, "Single byte: %f\n", cpu_time_used);
    #elif THOUSAND
        printf("Thousand byte\n");
        start = clock();
        thousand_byte(in, out, fileSize);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(time, "Thousand byte: %f\n", cpu_time_used);
    #endif
    fclose(in);
    fclose(out);
    fclose(time);

    return 0;  
}