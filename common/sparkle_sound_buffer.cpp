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

const uint64_t periodSize = 16384;
const uint64_t bufferSize = 65536;
const uint64_t periodTime = periodSize * 1000000000ULL / 44100;

/* ================================================================================================================== */

struct SparkleSoundBufferHeader
{
    uint64_t sec;
    uint64_t nsec;
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
    writePosition_ = 0;

    if (owner_)
        shmId_ = shmget_(key_, sizeof(SparkleSoundBufferHeader) + size_, IPC_CREAT | 0666);
    else
        shmId_ = shmget_(key_, sizeof(SparkleSoundBufferHeader) + size_, 0666);

    if (shmId_ == -1)
        throw WereException("[%p][%s] shmget failed.", this, __PRETTY_FUNCTION__);

    data_ = reinterpret_cast<unsigned char *>(shmat_(shmId_, NULL, 0));
    if (data_ == reinterpret_cast<unsigned char *>(-1))
        throw WereException("[%p][%s] shmat failed.", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */

unsigned char *SparkleSoundBuffer::data()
{
    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);

    return header->data;
}

void SparkleSoundBuffer::write(const void *data, int size, int *r_size)
{
    int max = size_ - writePosition_;
    if (size > max)
        size = max;
    memcpy(this->data() + writePosition_, data, size);
    writePosition_ = (writePosition_ + size) % size_;
    *r_size = size;
}

void SparkleSoundBuffer::get(void **data, int size, int *r_size, bool forward)
{
    int pos = readPosition(forward);
    int max = size_ - pos;
    if (size > max)
        size = max;
    *data = this->data() + pos;
    *r_size = size;
}

void SparkleSoundBuffer::start()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);
    header->sec = now.tv_sec;
    header->nsec = now.tv_nsec;
}

void SparkleSoundBuffer::stop()
{
    writePosition_ = 0;
}

int SparkleSoundBuffer::readPosition(bool forward)
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    SparkleSoundBufferHeader *header = reinterpret_cast<SparkleSoundBufferHeader *>(data_);

    uint64_t elapsed = 0;
    elapsed += 1000000000ULL * (now.tv_sec - header->sec);
    elapsed += now.tv_nsec - header->nsec;

    if (forward)
        elapsed += periodTime - 1;

    elapsed /= periodTime;

    int frames = (elapsed * periodSize) % bufferSize;

    return frames * 4;
}

/* ================================================================================================================== */
