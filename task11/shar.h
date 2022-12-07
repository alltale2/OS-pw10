#ifndef SHAR_H_
#define SHAR_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_PERM 0600

int shm_create(key_t shmkey, size_t size);
int shm_open(key_t shmkey);
void * shm_connect(int shmid);
int shm_disconnect(void * shmbuf);
int shm_del(int shmid);
pid_t shm_creator(int shmid);
size_t shm_size(int shmid);

#endif
