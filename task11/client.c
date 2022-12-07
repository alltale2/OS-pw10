#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "shared.h"
#include "semaphore.h"

void system_v_find(int *sem_score_id, int *sem_write_id, int *shm_id);
void work_data(int *buf);


int main(int argc, char **argv) {
    int sem_score_id, sem_write_id, shm_id;
    int first = 1;
    int *buf;

    setbuf(stdout, NULL);
    printf("Client Process [%ld] has started\n", (long)getpid());

    system_v_find(&sem_score_id, &sem_write_id, &shm_id);
    buf = (int *) shm_connect(shm_id);
    while(1) {
        sem_wait(sem_write_id);
        // must be one more semaphore - loch if process greater than one of different kind
        if (!first) {
            printf("Client: Result = %d\n", buf[0]);
        }
        work_data(buf);
        if (first) first = 0;
        sem_post(sem_score_id);
    }
    shm_disconnect(buf);
    printf("Client Process [%ld] has finished\n", (long)getpid());

    return EXIT_SUCCESS;
}


void system_v_find(int *sem_score_id, int *sem_write_id, int *shm_id) {
    key_t sem_score_key = IPC_PRIVATE, sem_write_key = IPC_PRIVATE, shm_key = IPC_PRIVATE;

    sem_score_key = ftok("./connect", 0);
    if (sem_score_key == -1) {
        fprintf(stderr, "Server: Error %d. Score semaphore key is not created\n",
                errno);
        exit(EXIT_FAILURE);
    }
    sem_write_key = ftok("./connect", 1);
    if (sem_write_key == -1) {
        fprintf(stderr, "Server: Error %d. Write semaphore key is not created\n",
                errno);
        exit(EXIT_FAILURE);
    }
    shm_key = ftok("./connect", 2);
    if (shm_key == -1) {
        fprintf(stderr, "Server: Error %d. Shared memory key is not created\n",
                errno);
        exit(EXIT_FAILURE);
    }

    if((*sem_score_id = sem_open(sem_score_key)) != -1){
        printf("Server: Score semaphore is opened: id %d\n", *sem_score_id);
    } else {
        printf("Server: Score semaphore is NOT opened\n");
    }

    if((*sem_write_id = sem_open(sem_write_key)) != -1){
        printf("Server: Write semaphore is opened: id %d\n", *sem_write_id);
    } else {
        printf("Server: Write semaphore is NOT opened\n");
    }

    if((*shm_id = shm_open(shm_key)) != -1){
        printf("Server: Shared memory is opened: id %d\n", *shm_id);
    } else {
        printf("Server: Shared memory is NOT opened\n");
    }
}

void work_data(int *buf) {
    int i, num, tmp;

    printf("Client (size < 0 to exit): Size = ");
    scanf("%d", &num);
    if (num < 0) exit(0);
    buf[0] = num;
    printf("Clent: num = %d\n", buf[0]);

    for(i = 1; i <= num; i++) {
        printf("a[%d]: ", i);
        scanf("%d", &tmp);
        buf[i] = tmp;
    }
}
