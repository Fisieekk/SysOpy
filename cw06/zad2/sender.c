#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>


typedef struct{
    double range_start;
    double range_end;
    unsigned long long n;
}specs;

int main(){
    specs specs;
    double result;

    int input_fd = open("int_fifo",O_WRONLY);
    if(input_fd == -1){
        perror("open");
        exit(1);
    }
    int output_fd = open("out_fifo",O_RDONLY);
    if(output_fd == -1){
        perror("open");
        exit(1);
    }

    while(1){

        printf("Integral range start: \n");
        if(scanf("%lf", &specs.range_start) < 0) {
            printf("Wrong range start\n");
            return -1;
        }
        printf("Integral range stop: \n");
        if(scanf("%lf", &specs.range_end) < 0) {
            printf("Wrong range stop\n");
            return -1;
        }
        printf("Integral number of intervals: \n");
        if(scanf("%llu", &specs.n) < 0) {
            printf("Wrong n\n");
            return -1;
        }
        printf("n = %llu\n",specs.n);

        if(write(input_fd, &specs, sizeof(specs)) < 0) {
            printf("Failed to write\n");
            return -1;
        }
        if(read(output_fd, &result, sizeof(result)) < 0) {
            printf("Failed to read\n");
            return -1;
        }
        printf("%lf\n",result);
    }

    close(input_fd);
    close(output_fd);
    return 0;

}