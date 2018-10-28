#include "sparkle_surface_shm.h"
#include <unistd.h>
#if 0
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include "shm/shm.h"
#endif

/* ================================================================================================================== */

SparkleSurfaceShm::~SparkleSurfaceShm()
{
    if (shmdt_(data_) == -1)
        were_debug("[%p][%s] shmdt failed.\n", this, __PRETTY_FUNCTION__);

    if (owner_)
        if (shmctl_(shmId_, IPC_RMID, NULL) == -1)
            were_debug("[%p][%s] shmctl failed.\n", this, __PRETTY_FUNCTION__);
}

SparkleSurfaceShm::SparkleSurfaceShm(key_t key, int width, int height, bool owner)
{
    width_ = width;
    height_ = height;
    owner_ = owner;
    key_ = key;
    shmId_ = -1;
    data_ = nullptr;

    if (owner_)
        shmId_ = shmget_(key_, width * height * 4, IPC_CREAT | 0666);
    else
        shmId_ = shmget_(key_, width * height * 4, 0666);

    if (shmId_ == -1)
        throw WereException("[%p][%s] shmget failed.", this, __PRETTY_FUNCTION__);

    data_ = reinterpret_cast<unsigned char *>(shmat_(shmId_, NULL, 0));
    if (data_ == reinterpret_cast<unsigned char *>(-1))
        throw WereException("[%p][%s] shmat failed.", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */
