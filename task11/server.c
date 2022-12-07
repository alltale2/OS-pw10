#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "shared.h"
#include "semaphore.h"

volatile sig_atomic_t stop_flag = 0;

void handler(int sig);
void system_v_start(int size, int *sem_score_id, int *sem_write_id, int *shm_id);
void system_v_stop(int sem_score_id, int sem_write_id, int shm_id);
void make_score(int *buf);


int main(int argc, char **argv) {
    size_t size = 100*sizeof(int);
    int sem_score_id, sem_write_id, shm_id;
    int *buf;

    setbuf(stdout, NULL);

    if (argc == 2) {
        size = (size_t) (atoi(argv[1]) * sizeof(int));
    }

    printf("\tServer Process [%ld] has started\n", (long)getpid());

    signal(SIGTERM, &handler);
    signal(SIGINT, &handler);

    system_v_start(size, &sem_score_id, &sem_write_id, &shm_id);
    buf = (int *) shm_connect(shm_id);

    while(!stop_flag) {
        printf("Server: Selver loop iteration\n");
        /*Start Work*/
        sem_wait(sem_score_id);
        // must be one more semaphore - loch if pocesses greater than one of different kind
        //sleep(3);
        //if (!stop_flag) {
        make_score(buf);
        sem_post(sem_write_id);
        //}
        /*End Work*/
    }

    shm_disconnect(buf);
    system_v_stop(sem_score_id, sem_write_id, shm_id);

    printf("\tServer Process [%ld] has finished\n", (long)getpid());

    return EXIT_SUCCESS;
}


void  handler(int sig) {
    printf("\tServer: Terminated signal catched\n");
    stop_flag = 1;
    //It is neccessary to make much correct stop procedure -
    //interrupted system call - wait on semaphore
}

void system_v_start(int size, int *sem_score_id, int *sem_write_id, int *shm_id) {
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

    if((*sem_score_id = sem_create(sem_score_key, 0)) != -1){
        printf("Server: Score semaphore is created: id %d\n", *sem_score_id);
    } else {
        printf("Server: Score semaphore is NOT created\n");
    }

    if((*sem_write_id = sem_create(sem_write_key, 1)) != -1){
        printf("Server: Write semaphore is created: id %d\n", *sem_write_id);
    } else {
        printf("Server: Write semaphore is NOT created\n");
    }

    if((*shm_id = shm_create(shm_key, size)) != -1){
        printf("Server: Shared memory is created: id %d\n", *shm_id);
    } else {
        printf("Server: Shared memory is NOT created\n");
    }
}


void system_v_stop(int sem_score_id, int sem_write_id, int shm_id) {
    if (shm_del(shm_id) != -1) {
        printf("Server: Shared memory (id %d) is deleted.\n", shm_id);
    } else {
        printf("Server: Shared memory (id %d) is NOT created\n", shm_id);
    }

    if (sem_del(sem_score_id) != -1) {
        printf("Server: Score semaphore (id %d) is deleted.\n", sem_score_id);
    } else {
        printf("Server: Score semaphore (id %d) is NOT created\n", sem_score_id);
    }

    if (sem_del(sem_write_id) != -1) {
        printf("Server: Write semaphore (id %d) is deleted.\n", sem_write_id);
    } else {
        printf("Server: Write semaphore (id %d) is NOT created\n", sem_write_id);
    }
}

void make_score(int *buf) {
    int i, num, res;

    num = buf[0];
    printf("\tServer: num = %d\n", num);
    buf[0] = 0;
    res = 0;
    for(i = 1; i <= num; i++) {
        res += buf[i];
        buf[i] = 0;
    }
    buf[0] = res;
    printf("\tServer: res = %d\n", buf[0]);
}
