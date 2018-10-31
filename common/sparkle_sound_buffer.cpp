#include "sparkle_sound_buffer.h"
#include <unistd.h>
#if 0
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include "shm/shm.h"
#endif
#include <cstring>

/* ================================================================================================================== */

struct SparkleSoundBufferHeader
{
    int writePosition;
    int readPosition;
    unsigned char data[0];
};

/* ================================================================================================================== */

SparkleSoundBuffer::~SparkleSoundBuffer()
{
    if (shmdt_(data_) == -1)
        were_debug("[%p][%s] shmdt failed.\n", this, __PRETTY_FUNCTION__);

    if (owner_)
        if (shmctl_(shmId_, IPC_RMID, NULL) == -1)
            were_debug("[%p][%s] shmctl failed.\n", this, __PRETTY_FUNCTION__);
}

SparkleSoundBuffer::SparkleSoundBuffer(key_t key, int size, bool owner)
{
    size_ = size;
    owner_ = owner;
    key_ = key;
    shmId_ = -1;
    data_ = nullptr;

    if (owner_)
        shmId_ = shmget_(key_, sizeof(SparkleSoundBufferHeader) + size_, IPC_CREAT | 0666);
    else
        shmId_ = shmget_(key_, sizeof(SparkleSoundBufferHeader) + size_, 0666);

    if (shmId_ == -1)
        throw WereException("[%p][%s] shmget failed.", this, __PRETTY_FUNCTION__);

    data_ = reinterpret_cast<unsigned char *>(shmat_(shmId_, NULL, 0));
    if (data_ == reinterpret_cast<unsigned char *>(-1))
        throw WereException("[%p][%s] shmat failed.", this, __PRETTY_FUNCTION__);

    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);
    header->writePosition = 0;
    header->readPosition = 0;
}

/* ================================================================================================================== */

unsigned char *SparkleSoundBuffer::data()
{
    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);

    return header->data;
}

int *SparkleSoundBuffer::writePosition()
{
    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);

    return &header->writePosition;
}

int *SparkleSoundBuffer::readPosition()
{
    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);

    return &header->readPosition;
}

void SparkleSoundBuffer::write(const void *data, int size, int *r_size)
{
    int *pos = writePosition();
    int max = size_ - *pos;
    if (size > max)
        size = max;
    memcpy(this->data() + *pos, data, size);
    *pos = (*pos + size) % size_;
    *r_size = size;
}

void SparkleSoundBuffer::get(void **data, int size, int *r_size)
{
    int *pos = readPosition();
    int max = size_ - *pos;
    if (size > max)
        size = max;
    *data = this->data() + *pos;
    *r_size = size;
}

/* ================================================================================================================== */
