#include "were_stream.h"
#include <cstring>

/* ================================================================================================================== */

WereStream::~WereStream()
{
}

WereStream::WereStream(std::vector<unsigned char> *vector)
{
    vector_ = vector;
    writePosition_ = 0;
    readPosition_ = 0;
}

/* ================================================================================================================== */

unsigned char *WereStream::allocate(unsigned int size)
{
    vector_->resize(vector_->size() + size);
    unsigned char *p = vector_->data() + (vector_->size() - size);
    return p;
}

void WereStream::add(const unsigned char *data, unsigned int size)
{
    unsigned char *p = allocate(size);
    memcpy(p, data, size);
    writePosition_ += size;
}

const unsigned char *WereStream::get(unsigned int size)
{
    const unsigned char *p = vector_->data() + readPosition_;
    readPosition_ += size;
    return p;
}

void WereStream::write(const void *data, unsigned int size)
{
    add(reinterpret_cast<const unsigned char *>(data), size);
}

void WereStream::read(void *data, unsigned int size)
{
    const unsigned char *p = get(size);
    memcpy(data, p, size);
}

/* ================================================================================================================== */

WereStream &operator<<(WereStream &stream, uint32_t data)
{
    stream.write(&data, sizeof(uint32_t));
    return stream;
}

WereStream &operator>>(WereStream &stream, uint32_t &data)
{
    stream.read(&data, sizeof(uint32_t));
    return stream;
}

WereStream &operator<<(WereStream &stream, int32_t data)
{
    stream.write(&data, sizeof(int32_t));
    return stream;
}

WereStream &operator>>(WereStream &stream, int32_t &data)
{
    stream.read(&data, sizeof(int32_t));
    return stream;
}

WereStream &operator<<(WereStream &stream, const std::vector<char> &data)
{
    uint32_t size = data.size();
    stream << size;
    stream.write(data.data(), size);
    return stream;
}

WereStream &operator>>(WereStream &stream, std::vector<char> &data)
{
    uint32_t size;
    stream >> size;
    data.resize(size);
    stream.read(data.data(), size);
    return stream;
}

/* ================================================================================================================== */
