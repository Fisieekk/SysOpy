#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>


int main(int argc, char* argv[])
{

 if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
 }

    //utworz proces potomny w ktorym wykonasz program ./calc z parametrami argv[1], argv[2] oraz argv[3]
    //odpowiednio jako pierwszy operand, operacja (+-x/) i drugi operand 
    //uzyj tablicowego sposobu przekazywania parametrow do programu 

      char* args[5]; 
      args[0] = "calc";
      args[1] = argv[1];
      args[2] = argv[2];
      args[3] = argv[3];
      args[4] = NULL;

      pid_t pid;
      if((pid = fork()) == 0){
        execvp("./calc", args);
        exit(1);
      }
 return 0;
}
