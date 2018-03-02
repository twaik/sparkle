#include "sparkle_packet.h"
#include <string.h>

#include "were/were.h"

//==================================================================================================

const WerePacker p_uint32 =
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

const WerePacker p_string =
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

void SparklePacketStream::write(const void *data, unsigned int size)
{
    _packet->add(reinterpret_cast<const unsigned char *>(data), size);
    _writePosition += size;
}

void SparklePacketStream::read(void *data, unsigned int size)
{
    memcpy(data, _packet->data() + _readPosition, size);
    _readPosition += size;
}

void SparklePacketStream::pWrite(const WerePacker *packer, void *data)
{
    packer->pack(this, data);
}

void SparklePacketStream::pRead(const WerePacker *packer, void *data)
{
    packer->unpack(this, data);
}

//==================================================================================================

SparklePacket *sparkle_packet_create()
{
    return new SparklePacket();
}

void sparkle_packet_destroy(SparklePacket *packet)
{
    delete packet;
}

unsigned char *sparkle_packet_data(SparklePacket *packet)
{
    return packet->data();
}

unsigned int sparkle_packet_size(SparklePacket *packet)
{
    return packet->size();
}

SparklePacketHeader *sparkle_packet_header(SparklePacket *packet)
{
    return packet->header();
}

//==================================================================================================

