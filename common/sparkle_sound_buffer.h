#ifndef SPARKLE_SOUND_BUFFER_H
#define SPARKLE_SOUND_BUFFER_H

#include <were/were.h>

/* ================================================================================================================== */

class SparkleSoundBuffer
{
public:
    ~SparkleSoundBuffer();
    SparkleSoundBuffer(key_t key, int size, bool owner);

    key_t key() {return key_;}
    int size() {return size_;}
    void write(const void *data, int size, int *r_size);
    void get(void **data, int size, int *r_size);

    unsigned char *data();
    int *writePosition();
    int *readPosition();

private:

private:
    int size_;
    bool owner_;

    key_t key_;
    int shmId_;
    unsigned char *data_;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SOUND_BUFFER_H */
