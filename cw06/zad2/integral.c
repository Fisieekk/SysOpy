// Napisz program, który liczy numerycznie wartość całki oznaczonej tak jak w zadaniu 1. 
// Obliczenia powinny zostać przeprowadzone w ten sposób, że pierwszy program czyta ze standardowego wejścia przedział w jakim całka ma być policzona a następnie przesyła przez potok nazwany do programu drugiego wartości określające ten przedział. Drugi program po otrzymaniu informacji liczy całkę w otrzymanym przedziale i odsyła do programu pierwszego wynik obliczeń. 
// Po otrzymaniu wyniku obliczeń, program pierwszy wyświetla wynik. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define minimum(a, b) (a < b ? a : b)


typedef struct{
    double range_start;
    double range_end;
    unsigned long long n;
}specs;


double f(double x){
    return 4 / (x * x + 1);
}

double integral_cal(specs* specs, double (*fun)(double)){
    double sum = 0.0;
    double interval_width = (specs->range_end - specs->range_start)/specs->n;

    double interval_start = specs->range_start;
    printf("n = %llu\n",specs->n);
    for(unsigned long long i = 0; i < specs->n; i++){
        sum += fun((interval_start + interval_width/2.0));
        interval_start += interval_width;
    }
    return sum * interval_width;
}

int main(){
    double result;
    specs specs;
    if(mkfifo("int_fifo",0666)!=0){
        perror("mkfifo");
        exit(1);
    }
    if (mkfifo("out_fifo",0666)!=0){
        perror("mkfifo");
        exit(1);
    }
    int input_fd = open("int_fifo",O_RDONLY);
    if(input_fd == -1){
        perror("open");
        exit(1);
    }
    int output_fd = open("out_fifo",O_WRONLY);
    if(output_fd == -1){
        perror("open");
        exit(1);
    }
    while(read(input_fd,&specs,sizeof(specs))>0){
        result = integral_cal(&specs,f);
        write(output_fd,&result,sizeof(result));
    }
    
    close(input_fd);
    close(output_fd);

    return 0;
    
}