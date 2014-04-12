// 
// 
// 
// <Put your name and ID here>
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "schedule_ec.h"





//
// main - The simulator's main routine
//
int main(int argc, char **argv){
    
    for(int i=0;i<10;i++){
        addProcessEC(i,i%3);
    }  
    
    int process = 0;
    int remove = 0;
    for(int i=0;i<1000;i++){
        if(i!=0 && i%130==0){
            removeProcessEC(remove);
            remove++;
        }
        printf("Process %d executed\n", nextProcessEC());
    }
    
    
    exit(0); //control never reaches here
}
  










