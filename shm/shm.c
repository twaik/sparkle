#include "shm.h"
#include <unistd.h>
#include <sys/syscall.h>

#define IPCOP_semop      1
#define IPCOP_semget     2
#define IPCOP_semctl     3
#define IPCOP_msgsnd    11
#define IPCOP_msgrcv    12
#define IPCOP_msgget    13
#define IPCOP_msgctl    14
#define IPCOP_shmat     21
#define IPCOP_shmdt     22
#define IPCOP_shmget    23
#define IPCOP_shmctl    24

#define IPC_MODERN 0x100

#define SHMLBA 4096

#ifdef SYS_shmat
void *shmat_(int id, const void *addr, int flag)
{
    return (void *)syscall(SYS_shmat, id, addr, flag);
}
#else
void *shmat_(int id, const void *addr, int flag)
{
    unsigned long ret;
    ret = syscall(SYS_ipc, IPCOP_shmat, id, flag, &addr, addr);
    return (ret > -(unsigned long)SHMLBA) ? (void *)ret : (void *)addr;
}
#endif

//int shmctl_(int id, int cmd, struct shmid_ds *buf)
int shmctl_(int id, int cmd, void *buf)
{
    #ifdef SYS_shmctl
    return syscall(SYS_shmctl, id, cmd | IPC_MODERN, buf);
    #else
    return syscall(SYS_ipc, IPCOP_shmctl, id, cmd | IPC_MODERN, 0, buf, 0);
    #endif
}

int shmdt_(const void *addr)
{
    #ifdef SYS_shmdt
    return syscall(SYS_shmdt, addr);
    #else
    return syscall(SYS_ipc, IPCOP_shmdt, addr);
    #endif
}

int shmget_(key_t key, size_t size, int flag)
{
    #ifdef SYS_shmget
    return syscall(SYS_shmget, key, size, flag);
    #else
    return syscall(SYS_ipc, IPCOP_shmget, key, size, flag);
    #endif
}
