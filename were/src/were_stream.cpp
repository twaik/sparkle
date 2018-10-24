#include "were_stream.h"
#include <cstring>
#include <cstdint>

/* ================================================================================================================== */

WereStream::~WereStream()
{
}

WereStream::WereStream(std::vector<unsigned char> *vector)
{
    vector_ = vector;
    readPosition_ = 0;
    writePosition_ = 0;
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

void WereStream::pWrite(const WerePacker *packer, const void *data)
{
    packer->pack(this, data);
}

void WereStream::pRead(const WerePacker *packer, void *data)
{
    packer->unpack(this, data);
}

/* ================================================================================================================== */

const WerePacker p_uint32 =
{
    .pack = [](WereStream *stream, const void *data)
    {
        stream->write(data, sizeof(uint32_t));
    },
    .unpack = [](WereStream *stream, void *data)
    {
        stream->read(data, sizeof(uint32_t));
    },
};

const WerePacker p_string =
{
    .pack = [](WereStream *stream, const void *data)
    {
        const char * const *_data = reinterpret_cast<const char * const *>(data);
        uint32_t length;
        length = strlen(*_data);
        stream->write(&length, sizeof(uint32_t));
        stream->write(*_data, length + 1);
    },
    .unpack = [](WereStream *stream, void *data)
    {
        const char **_data = reinterpret_cast<const char **>(data);
        uint32_t length;
        stream->read(&length, sizeof(uint32_t));
        *_data = reinterpret_cast<const char *>(stream->get(length + 1));
    },
};

const WerePacker p_float =
{
    .pack = [](WereStream *stream, const void *data)
    {
        stream->write(data, sizeof(float));
    },
    .unpack = [](WereStream *stream, void *data)
    {
        stream->read(data, sizeof(float));
    },
};

/* ================================================================================================================== */
