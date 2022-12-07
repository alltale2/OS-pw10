#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/shm.h> 
#include <sys/sem.h> 

#define SHMEM_SIZE 4096 

void main_thread(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]);
void finish_process(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]);
void child_thread(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]);