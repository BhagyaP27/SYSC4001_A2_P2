/**
 * @file process2.c
 * @author Bhagya Patel
 * @brief Process launched via exec - decrements counter
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int counter = 0;
    int cycle = 0;
    
    printf("\n PROCESS 2 (via exec) \n");
    printf("PID: %d\n", getpid());
    printf("Parent PID: %d\n\n", getppid());
    
    while(1) {
        if (counter % 3 == 0) {
            printf("[Process 2] Cycle: %d - %d is a multiple of 3\n", cycle, counter);
        } else {
            printf("[Process 2] Cycle: %d\n", cycle);
        }
        counter--;
        cycle++;
        sleep(1);
    }
    
    return 0;
}