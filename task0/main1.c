#include "header.h"

#define SHMEM_SIZE 4096 

union semnum { 
    int val; 
    struct semid_ds *buf; 
    unsigned short * array; 
} sem_arg; 

int main(int argc, char ** argv) { 
  pid_t pid;
    int shm_id, sem_id; 
    int* shm_buf; 
    int shm_size; 
    struct shmid_ds ds; 
    struct sembuf sb[2]; 
    unsigned short sem_vals[2]; 
         
    shm_id = shmget(IPC_PRIVATE, SHMEM_SIZE, 
                         IPC_CREAT | IPC_EXCL | 0600); 
    if (shm_id == -1) { 
        fprintf(stderr, "shmget() error\n"); 
        return EXIT_FAILURE; 
    } 
         
    sem_id = semget(IPC_PRIVATE, 2, 
                    0600 | IPC_CREAT | IPC_EXCL); 
    if (sem_id == -1) { 
        fprintf(stderr, "semget() error\n"); 
        return EXIT_FAILURE; 
    } 
         
    printf("Semaphore: %d\n", sem_id); 
    sem_vals[0] = 1; 
    sem_vals[1] = 1; 
    sem_arg.array = sem_vals; 
         
    if (semctl(sem_id, 0, SETALL, sem_arg) == -1) { 
        fprintf(stderr, "semctl() error\n"); 
        return EXIT_FAILURE; 
    } 
         
    shm_buf = (int*) shmat(shm_id, NULL, 0); 
    if (shm_buf == (int*) -1) { 
        fprintf(stderr, "shmat() error\n"); 
        return EXIT_FAILURE; 
    } 
         
    shmctl(shm_id, IPC_STAT, &ds); 
    shm_size = ds.shm_segsz; 

    pid = fork();
    if(pid == 0){
      child_thread(shm_size, sem_id, shm_buf, sb);
    }
    else if(pid == -1){
      printf("Fork error!\n");
      exit(1);
    }
    else{
      sleep(1);
      main_thread(shm_size, sem_id, shm_buf, sb);

      sb[0].sem_num = 0; 
      sb[0].sem_flg = SEM_UNDO; 
      sb[0].sem_op = -1; 

      semop(sem_id, sb, 1); 
           
      semctl(sem_id, 1, IPC_RMID, sem_arg); 
      shmdt(shm_buf); 
      shmctl(shm_id, IPC_RMID, NULL); 
    }
         
    return EXIT_SUCCESS; 
}

void main_thread(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]){
  int exit_cond = 0;
  int n;
  int arr[mem_size/sizeof(int)];
  while(!exit_cond) {
    sb[0].sem_num = 0; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = -1;

    semop(sem_id, sb, 1); 

    if(shm_buf[0] != 0){
      printf("Sum = %d\n", shm_buf[0]);
    }

    sb[0].sem_num = 0; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = 1;

    semop(sem_id, sb, 1);
    
    printf("Enter n:");
    scanf("%d", &n);
    if(n == -1){
      exit_cond = 1;
      finish_process(mem_size, sem_id, shm_buf, sb);
      continue;
    }
    if(mem_size/4 - 1 < n){
      printf("arr is to large. n sets 10\n");
      n = 10;
    }
    arr[0] = n;
    printf("Enter arr elements:\n");
    for(int i = 0; i < n; i++){
      printf("%d: ", i);
      scanf("%d", &arr[i+1]);
    }

    sb[0].sem_num = 0; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = -1;

    semop(sem_id, sb, 1); 
    //crit sec
    for(int i = 0; i <= n; i++){
      shm_buf[i] = arr[i];
    }
    //end of crit sec

    sb[0].sem_num = 1; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = 1;

    semop(sem_id, sb, 1);
    sleep(1);
  }
}

void finish_process(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]){
    sb[0].sem_num = 0; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = -1;
    semop(sem_id, sb, 1); 

    shm_buf[0] = -1;

    sb[0].sem_num = 1; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = 1;
}

void child_thread(int mem_size, int sem_id, int* shm_buf, struct sembuf sb[2]){
  printf("Hello from child\n");
  int exit_cond = 0;
  int sum;

  int arr[mem_size/sizeof(int)];
  while(!exit_cond) {
    sum = 0;

    sb[0].sem_num = 1; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = -1;

    semop(sem_id, sb, 1); 

    //crit sec
    if(shm_buf[0] == -1){
      exit_cond = 1;
      continue;
    }

    for(int i = 0; i < shm_buf[0]; i++){
      sum += shm_buf[i+1];
    }

    shm_buf[0] = sum;
    //end of crit sec
    
    sb[0].sem_num = 0; 
    sb[0].sem_flg = 0; 
    sb[0].sem_op = 1;

    semop(sem_id, sb, 1);
    sleep(1);
  }
  sb[0].sem_num = 0; 
  sb[0].sem_flg = SEM_UNDO; 
  sb[0].sem_op = 1; 

  semop(sem_id, sb, 1); 
}