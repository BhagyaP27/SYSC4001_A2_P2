/**
 * @file process1.c
 * @brief SYSC 4001 Assignment  2 - Part II: Concurrent Processes
 * @author Bhagya Patel
 * @author Tomas Alvarez
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct shared_data {
    int multiple;
    int counter;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void part1_simple_counter(void);
void part2_multiples_with_exec(void);
void part3_with_wait(void);
void part4_shared_memory(void);
void part5_with_semaphores(void);
void sem_wait(int semid);
void sem_signal(int semid);

int main(int argc, char *argv[]) {
    int choice;
    
    printf("  SYSC 4001 Assignment 2 - Part II\n");
    printf("Select which part to run:\n");
    printf("  1. Part 1: Simple fork\n");
    printf("  2. Part 2: Exec\n");
    printf("  3. Part 3: Wait\n");
    printf("  4. Part 4: Shared memory\n");
    printf("  5. Part 5: Semaphores\n");
    printf("\nEnter choice (1-5): ");
    
    scanf("%d", &choice);
    printf("\n");
    
    switch(choice) {
        case 1: part1_simple_counter(); break;
        case 2: part2_multiples_with_exec(); break;
        case 3: part3_with_wait(); break;
        case 4: part4_shared_memory(); break;
        case 5: part5_with_semaphores(); break;
        default: printf("Invalid!\n"); return 1;
    }
    return 0;
}

void part1_simple_counter(void) {
    pid_t pid;
    int counter = 0;
    
    printf("PART 1: Simple Fork\n");
    printf("Press Ctrl+C to stop\n\n");
    
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("[CHILD] Process 2 started (PID: %d)\n\n", getpid());
        while(1) {
            printf("[Process 2 - PID %d] Counter: %d\n", getpid(), counter);
            counter++;
            sleep(1);
        }
    }
    else {
        printf("[PARENT] Process 1 started (PID: %d)\n", getpid());
        printf("[PARENT] Child PID: %d\n\n", pid);
        while(1) {
            printf("[Process 1 - PID %d] Counter: %d\n", getpid(), counter);
            counter++;
            sleep(1);
        }
    }
}

void part2_multiples_with_exec(void) {
    pid_t pid;
    int counter = 0, cycle = 0;
    
    printf("=== PART 2: Multiples with Exec ===\n");
    printf("Press Ctrl+C to stop\n\n");
    
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("[CHILD] Launching process2 via exec...\n\n");
        execl("./process2", "process2", NULL);
        perror("ERROR: exec failed");
        exit(1);
    }
    else {
        printf("[PARENT] Process 1 started (PID: %d)\n\n", getpid());
        while(1) {
            if (counter % 3 == 0) {
                printf("[Process 1] Cycle: %d - %d is a multiple of 3\n", cycle, counter);
            } else {
                printf("[Process 1] Cycle: %d\n", cycle);
            }
            counter++;
            cycle++;
            sleep(1);
        }
    }
}

void part3_with_wait(void) {
    pid_t pid;
    int counter = 0, cycle = 0, status;
    
    printf("=== PART 3: With Wait ===\n");
    printf("Process 2 runs until counter < -500\n\n");
    
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("[CHILD] Process 2 started (PID: %d)\n\n", getpid());
        while(counter > -500) {
            if (counter % 3 == 0) {
                printf("[Process 2] Cycle: %d - %d is a multiple of 3\n", cycle, counter);
            } else {
                printf("[Process 2] Cycle: %d\n", cycle);
            }
            counter--;
            cycle++;
            usleep(10000);
        }
        printf("\n[Process 2] Reached -500, terminating\n");
        exit(0);
    }
    else {
        printf("[PARENT] Process 1 waiting for child...\n\n");
        waitpid(pid, &status, 0);
        printf("\n[Process 1] Child finished, parent terminating\n");
    }
}

void part4_shared_memory(void) {
    pid_t pid;
    int shmid;
    struct shared_data *shared;
    key_t key = 1234;
    
    printf(" PART 4: Shared Memory \n");
    printf("Process 2 starts when counter > 100\n");
    printf("Both finish when counter > 500\n\n");
    
    shmid = shmget(key, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    
    shared = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared == (struct shared_data *) -1) {
        perror("shmat failed");
        exit(1);
    }
    
    shared->multiple = 3;
    shared->counter = 0;
    
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("[CHILD] Waiting for counter > 100...\n\n");
        while(shared->counter <= 100) {
            usleep(10000);
        }
        printf("[Process 2] Counter reached 100!\n\n");
        
        while(shared->counter <= 500) {
            if (shared->counter % shared->multiple == 0) {
                printf("[Process 2] Counter: %d is a multiple of %d\n",
                       shared->counter, shared->multiple);
            }
            usleep(50000);
        }
        printf("\n[Process 2] Counter > 500, terminating\n");
        shmdt(shared);
        exit(0);
    }
    else {
        printf("[PARENT] Process 1 started\n\n");
        while(shared->counter <= 500) {
            if (shared->counter % shared->multiple == 0) {
                printf("[Process 1] Counter: %d is a multiple of %d\n",
                       shared->counter, shared->multiple);
            }
            shared->counter++;
            usleep(50000);
        }
        printf("\n[Process 1] Counter > 500, terminating\n");
        wait(NULL);
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        printf("\n[CLEANUP] Shared memory removed\n");
    }
}

void part5_with_semaphores(void) {
    pid_t pid;
    int shmid, semid;
    struct shared_data *shared;
    key_t key = 5678;
    union semun arg;
    
    printf(" PART 5: With Semaphores \n");
    printf("Same as Part 4 but with mutex protection\n\n");
    
    shmid = shmget(key, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    
    shared = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared == (struct shared_data *) -1) {
        perror("shmat failed");
        exit(1);
    }
    
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(1);
    }
    
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) < 0) {
        perror("semctl failed");
        exit(1);
    }
    
    shared->multiple = 3;
    shared->counter = 0;
    
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("[CHILD] Waiting for counter > 100...\n\n");
        int local;
        do {
            sem_wait(semid);
            local = shared->counter;
            sem_signal(semid);
            usleep(10000);
        } while(local <= 100);
        
        printf("[Process 2] Counter reached 100!\n\n");
        
        while(1) {
            sem_wait(semid);
            local = shared->counter;
            if (local > 500) {
                sem_signal(semid);
                break;
            }
            if (local % shared->multiple == 0) {
                printf("[Process 2] Protected counter: %d is a multiple of %d\n",
                       local, shared->multiple);
            }
            sem_signal(semid);
            usleep(50000);
        }
        printf("\n[Process 2] Counter > 500, terminating\n");
        shmdt(shared);
        exit(0);
    }
    else {
        printf("[PARENT] Process 1 started\n\n");
        while(1) {
            sem_wait(semid);
            if (shared->counter > 500) {
                sem_signal(semid);
                break;
            }
            if (shared->counter % shared->multiple == 0) {
                printf("[Process 1] Protected counter: %d is a multiple of %d\n",
                       shared->counter, shared->multiple);
            }
            shared->counter++;
            sem_signal(semid);
            usleep(50000);
        }
        printf("\n[Process 1] Counter > 500, terminating\n");
        wait(NULL);
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID, arg);
        printf("\n[CLEANUP] Shared memory and semaphore removed\n");
    }
}

void sem_wait(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

void sem_signal(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}