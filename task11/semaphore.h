#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define SEM_PERM 0600

typedef union {
    int val;
    struct semid_ds *buf;
    ushort *array;
} semun;

int sem_create(key_t semkey, int value);
int sem_open(key_t semkey);
int sem_wait(int semid);
int sem_post(int semid);
int sem_del(int semid);

#endif
