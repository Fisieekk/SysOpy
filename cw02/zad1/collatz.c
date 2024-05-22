#include "collatz.h"


int collatz_conjecture(int input){
    if(input % 2 == 0){
        input /= 2;
    } else {
        input = 3 * input + 1;
    }
    return input;
}
int test_collatz_convergence(int input, int max_iter){
    int steps = 1;
    while(steps < max_iter){
        if(input == 1){
           return steps;
        }
        input=collatz_conjecture(input);
        steps++;
    }
    return steps;
}