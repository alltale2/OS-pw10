#include <stdio.h>
#include <stdlib.h>
#include "semaphore.h"

int sem_create(key_t semkey, int value) {
    int status = 0, semid;

    if ((semid = semget(semkey, 1,
                        SEM_PERM | IPC_CREAT | IPC_EXCL)) != -1) {
        semun arg;
        arg.val = value;
        status = semctl(semid, 0, SETVAL, arg);
    }

    if (semid == -1 || status == -1) {
        perror("\tsem_create invocation ERROR");
        return -1;
    }

    /* All OK */
    return semid;
}


int sem_open(key_t semkey) {

    int semid = semget(semkey, 1, 0);

    if (semid == -1) {
        perror("\tsem_open invocation ERROR");
        return -1; // better exit(1);
    }

    return semid;
}


int sem_wait(int semid) {
    struct sembuf p_buf;

    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = SEM_UNDO;

    if (semop(semid, &p_buf, 1) == -1) {
        perror("\tsem_wait invocation error"); 	//sem_wait invocation error: Interrupted system call -
        //when wait is interrupted by a signal see "sigaction" to repeat interrupted system call
        return -1; //exit(1); // to prevent stop program after that
    }

    return 0;
}


int sem_post(int semid) {
    struct sembuf p_buf;

    p_buf.sem_num = 0;
    p_buf.sem_op = 1;
    p_buf.sem_flg = SEM_UNDO;

    if (semop(semid, &p_buf, 1) == -1) {
        /* It was == 1, but may be error */
        perror("\tsem_post invocation error");
        return -1; //exit(1);
    }

    return 0;
}

int sem_del(int semid) {
    int status = 0;

    status = semctl(semid, 0, IPC_RMID);

    if (status == -1) {
        perror("\tdel_sem invocation ERROR");
        return -1;
    }

    return 0;
}
