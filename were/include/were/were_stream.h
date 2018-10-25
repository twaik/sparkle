#ifndef WERE_STREAM_H
#define WERE_STREAM_H

#include <vector>
#include <cstdint>

/* ================================================================================================================== */

class WereStream
{
public:
    ~WereStream();
    WereStream(std::vector<unsigned char> *vector);

    void add(const unsigned char *data, unsigned int size);
    const unsigned char *get(unsigned int size);

    void write(const void *data, unsigned int size);
    void read(void *data, unsigned int size);

private:
    unsigned char *allocate(unsigned int size);

private:
    std::vector<unsigned char> *vector_;
    unsigned int writePosition_;
    unsigned int readPosition_;
};

/* ================================================================================================================== */

WereStream &operator<<(WereStream &stream, uint32_t data);
WereStream &operator>>(WereStream &stream, uint32_t &data);
WereStream &operator<<(WereStream &stream, int32_t data);
WereStream &operator>>(WereStream &stream, int32_t &data);
WereStream &operator<<(WereStream &stream, const std::vector<char> &data);
WereStream &operator>>(WereStream &stream, std::vector<char> &data);

/* ================================================================================================================== */

#endif /* WERE_STREAM_H */
