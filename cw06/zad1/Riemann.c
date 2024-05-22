// Napisz program, który liczy numerycznie wartość całki oznaczonej z funkcji 4/(x2+1) w przedziale od 0 do 1 metodą prostokątów (z definicji całki oznaczonej Riemanna). 
// Pierwszy parametr programu to szerokość każdego prostokąta, określająca dokładność obliczeń. 
// Obliczenia należy rozdzielić na n procesów potomnych (n drugi parametr wywołania programu), tak by każdy z procesów liczył inny fragment ustalonego wyżej przedziału. 
// Każdy z procesów powinien wynik swojej części obliczeń przesłać przez potok nienazwany do procesu macierzystego. 
// Każdy proces potomny do komunikacji z procesem macierzystym powinien używać osobnego potoku. 
// Proces macierzysty powinien oczekiwać na wyniki uzyskane od wszystkich procesów potomnych po czym powinien dodać te wyniki cząstkowe i wyświetlić wynik na standardowym wyjściu. W programie zmierz, wypisz na konsolę i zapisz do pliku z raportem czasy realizacji dla różnej liczby procesów potomnych oraz różnych dokładności obliczeń. 
// Dokładności obliczeń należy dobrać w ten sposób by obliczenia trwały co najmniej kilka sekund.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <math.h>

#define MAX 1000

double f(double x)
{
    return 4 / (x * x + 1);
}

#define minimum(a, b) (a < b ? a : b)


double integral_cal(double interval_start, double interval_end,double (*function)(double), double h,unsigned long long n){
    if(interval_end-interval_start < h){
        return function((interval_start+interval_end)/2)*(interval_end-interval_start);
    }
    double sum = 0;
    for(int i = 0; i < n; i++){
        sum += function((interval_start + h/2));
        interval_start += h;
    }
    return sum*h;
}
double range_start=0;
double range_end=1;
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Niepoprawna liczba argumentów\n");
        return 1;
    }

    int n = atoi(argv[2]);
    double h = atof(argv[1]);
    if(ceil((range_end-range_start)/h) < n){
        printf("Za mała dokładność\n");
        return 1;
    }
    unsigned long long counter =  ((unsigned long long )ceil((double)(range_end-range_start)/h))/n;
    double interval_start = range_start;
    double interval_end = range_start;
    int fd[n][2];

    for(int i=0;i<n;i++){
        interval_end = minimum(range_end,interval_start + h*counter);
        if(pipe(fd[i]) == -1){
            perror("pipe");
            return 1;
        }
        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            return 1;
        }
        if(pid == 0){
            double integral_result = integral_cal(interval_start,interval_end,f,h,counter);
            close(fd[i][0]);
            if(write(fd[i][1],&integral_result,sizeof(integral_result))<0){
                perror("write");
                return 1;
            }
            exit(0);
        }
        close(fd[i][1]);
        interval_start = interval_end;
        // printf("%f %f\n",interval_start,interval_end);
    }
    double sum = 0;
    for (int i = 0; i < n; i++){
        double result;
        if(read(fd[i][0],&result,sizeof(result)) == -1){
            perror("read");
            return 1;
        }
        sum += result;
    }
    printf("Wynik: %f\n",sum);
    return 0;
}