#ifndef SION_SHM_H
#define SION_SHM_H

#include <sys/types.h>

#define IPC_CREAT  01000
#define IPC_EXCL   02000
#define IPC_NOWAIT 04000

#define IPC_RMID 0
#define IPC_SET  1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_PRIVATE ((key_t) 0)

#ifdef __cplusplus
extern "C" {
#endif

void *shmat_(int, const void *, int);
//int shmctl_(int, int, struct shmid_ds *);
int shmctl_(int, int, void *);
int shmdt_(const void *);
int shmget_(key_t, size_t, int);

#ifdef __cplusplus
}
#endif

#endif /* SION_SHM_H */
