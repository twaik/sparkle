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
}

SparklePacket::SparklePacket()
{
    _header = {};
}

unsigned char *SparklePacket::data()
{
    return _data.data();
}

unsigned int SparklePacket::size() const
{
    return _data.size();
}

SparklePacketHeader *SparklePacket::header()
{
    return &_header;
}

unsigned char *SparklePacket::allocate(unsigned int size)
{
    _data.resize(_data.size() + size);
    unsigned char *p = _data.data() + (_data.size() - size);
    return p;
}

void SparklePacket::add(const unsigned char *bytes, unsigned int size)
{
    unsigned char *p = allocate(size);
    memcpy(p, bytes, size);
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

sparkle_packet_t *sparkle_packet_create()
{
    SparklePacket *_packet = new SparklePacket();
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

//==================================================================================================

