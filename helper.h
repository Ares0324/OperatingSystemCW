/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
# include <cstdlib>
using namespace std;

# define SEM_KEY 0x0F // Change this number as needed


union semun {
   int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
  ushort *array;         /* used for GETALL and SETALL */
};


int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);

//ERROR MESSAGE
#define INPUT_ERROR 1
#define SEMAPHORE_CREATION_ERROR 2
#define SEMAPHORE_INITIALIZATION_ERROR 3
#define THREAD_CREATION_ERROR 4
#define THREAD_JOIN_ERROR 5
#define SEMAPHORE_DESTROY_ERROR 6
