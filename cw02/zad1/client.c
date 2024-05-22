#include <stdio.h>
#ifndef DYNAMIC
#include "collatz.c"
#else
#include <dlfcn.h>
#endif


int main(){
    #ifdef DYNAMIC
    void *dlhandle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!dlhandle) {
        printf("Library Opening Error!\n");
        return 0;
    }
    int (*test_collatz_convergence)(int input, int max_iter);
    test_collatz_convergence = dlsym(dlhandle, "test_collatz_convergence");

    int input[5] = {5,13,54,16,123};
    int const max_iter = 100;
    for(int i=0; i<5; i++){
        printf("Collatz(%d) = %d\n", input[i], test_collatz_convergence(input[i], max_iter));
    }
    dlclose(dlhandle);    
    return 0;
    #else
    int input[5] = {5,13,54,16,27};
    int const max_iter = 1000;
    for(int i=0; i<5; i++){
        printf("Collatz(%d) = %d\n", input[i], test_collatz_convergence(input[i], max_iter));
    }
    return 0;
    #endif

}