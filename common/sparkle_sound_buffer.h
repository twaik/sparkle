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
    unsigned char *data();

    void write(const void *data, int size, int *r_size);
    void get(void **data, int size, int *r_size, bool forward = false);
    void start();
    void stop();
    int readPosition(bool forward = false);

private:

private:
    int size_;
    bool owner_;

    key_t key_;
    int shmId_;
    unsigned char *data_;

    int writePosition_;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SOUND_BUFFER_H */
