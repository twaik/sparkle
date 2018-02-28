#include "sparkle_packet.h"
#include <string.h>

#include "were/were.h"

//==================================================================================================

const packer_t p_uint32 =
{
    .pack = [](SparklePacketStream *stream, void *data)
    {
        stream->write(data, sizeof(uint32_t));
    },
    .unpack = [](SparklePacketStream *stream, void *data)
    {
        stream->read(data, sizeof(uint32_t));
    },
};

const packer_t p_string =
{
    .pack = [](SparklePacketStream *stream, void *data)
    {
        char **_data = reinterpret_cast<char **>(data);
        uint32_t length;
        length = strlen(*_data);
        stream->write(&length, sizeof(uint32_t));
        stream->write(*_data, length + 1);
    },
    .unpack = [](SparklePacketStream *stream, void *data)
    {
        const char **_data = reinterpret_cast<const char **>(data);
        uint32_t length;
        stream->read(&length, sizeof(uint32_t));
        *_data = reinterpret_cast<const char *>(stream->getData(length + 1));
    },
};

//==================================================================================================

SparklePacket::~SparklePacket()
{
    delete[] _data;
}

SparklePacket::SparklePacket(unsigned int maxSize)
{
    _maxSize = maxSize;
    _data = new unsigned char [maxSize];
    _size = 0;
}

unsigned char *SparklePacket::data()
{
    return _data;
}

unsigned int SparklePacket::size() const
{
    return _size;
}

SparklePacketHeader *SparklePacket::header()
{
    return &_header;
}

unsigned char *SparklePacket::allocate(unsigned int size)
{
    unsigned char *p = _data + _size;
    _size += size;
    return p;
}

void SparklePacket::add(const unsigned char *bytes, unsigned int size)
{
    memcpy(_data + _size, bytes, size);
    _size += size;
}

//==================================================================================================

SparklePacketStream::~SparklePacketStream()
{
}

SparklePacketStream::SparklePacketStream(SparklePacket *packet)
{
    _packet = packet;
    _readPosition = 0;
    _writePosition = 0;
}

//==================================================================================================

void SparklePacketStream::addData(const unsigned char *a, unsigned int size)
{
    _packet->add(a, size);
    _writePosition += size;
}

const unsigned char *SparklePacketStream::getData(unsigned int size)
{
    const unsigned char *p = _packet->data() + _readPosition;
    _readPosition += size;
    return p;
}

void SparklePacketStream::write(void *data, unsigned int size)
{
    _packet->add(reinterpret_cast<unsigned char *>(data), size);
    _writePosition += size;
}

void SparklePacketStream::read(void *data, unsigned int size)
{
    memcpy(data, _packet->data() + _readPosition, size);
    _readPosition += size;
}

void SparklePacketStream::pWrite(const packer_t *packer, void *data)
{
    packer->pack(this, data);
}

void SparklePacketStream::pRead(const packer_t *packer, void *data)
{
    packer->unpack(this, data);
}

//==================================================================================================

sparkle_packet_t *sparkle_packet_create(unsigned int size)
{
    SparklePacket *_packet = new SparklePacket(size);
    return _packet;
}

void sparkle_packet_destroy(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    delete _packet;
}

unsigned char *sparkle_packet_data(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->data();
}

unsigned int sparkle_packet_size(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->size();
}

SparklePacketHeader *sparkle_packet_header(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->header();
}

sparkle_packet_stream_t *sparkle_packet_stream_create(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    SparklePacketStream *_stream = new SparklePacketStream(_packet);
    return _stream;
}

void sparkle_packet_stream_destroy(sparkle_packet_stream_t *stream)
{
    SparklePacketStream *_stream = static_cast<SparklePacketStream *>(stream);
    delete _stream;
}

//==================================================================================================

