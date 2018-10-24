#ifndef WERE_STREAM_H
#define WERE_STREAM_H

#include <vector>

/* ================================================================================================================== */

struct WerePacker;

class WereStream
{
public:
    ~WereStream();
    WereStream(std::vector<unsigned char> *vector);

    void add(const unsigned char *data, unsigned int size);
    const unsigned char *get(unsigned int size);

    void write(const void *data, unsigned int size);
    void read(void *data, unsigned int size);

    void pWrite(const WerePacker *packer, const void *data);
    void pRead(const WerePacker *packer, void *data);

private:
    unsigned char *allocate(unsigned int size);

private:
    std::vector<unsigned char> *vector_;
    unsigned int readPosition_;
    unsigned int writePosition_;
};

struct WerePacker
{
    void (*pack)(WereStream *stream, const void *data);
    void (*unpack)(WereStream *stream, void *data);
};
typedef struct WerePacker WerePacker;

/* ================================================================================================================== */

extern const WerePacker p_uint32;
extern const WerePacker p_string;
extern const WerePacker p_float;

/* ================================================================================================================== */

#endif /* WERE_STREAM_H */
