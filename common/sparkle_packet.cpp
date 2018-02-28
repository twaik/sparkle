#include "sparkle_packet.h"
#include <string.h>

#include "were/were.h"

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

void SparklePacketStream::addUint32(uint32_t a)
{
    addData(reinterpret_cast<const unsigned char *>(&a), sizeof(uint32_t));
}

uint32_t SparklePacketStream::getUint32()
{
    const uint32_t *p = reinterpret_cast<const uint32_t *>(getData(sizeof(uint32_t)));
    return *p;
}

void SparklePacketStream::addString(const std::string &a)
{
    addUint32(a.length());
    addData(reinterpret_cast<const unsigned char *>(a.c_str()), a.length() + 1);
}

std::string SparklePacketStream::getString()
{
    uint32_t length = getUint32();
    const char *p = reinterpret_cast<const char*>(getData(length + 1));
    return std::string(p, length);
}

const char *SparklePacketStream::getStringPointer()
{
    uint32_t length = getUint32();
    const char *p = reinterpret_cast<const char*>(getData(length + 1));
    return p;
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

uint32_t sparkle_packet_stream_get_uint32(sparkle_packet_stream_t *stream)
{
    SparklePacketStream *_stream = static_cast<SparklePacketStream *>(stream);
    return _stream->getUint32();
}

const char *sparkle_packet_stream_get_string(sparkle_packet_stream_t *stream)
{
    SparklePacketStream *_stream = static_cast<SparklePacketStream *>(stream);
    return _stream->getStringPointer();
}

void sparkle_packet_stream_add_uint32(sparkle_packet_stream_t *stream, uint32_t a)
{
    SparklePacketStream *_stream = static_cast<SparklePacketStream *>(stream);
    _stream->addUint32(a);
}

void sparkle_packet_stream_add_string(sparkle_packet_stream_t *stream, const char *a)
{
    SparklePacketStream *_stream = static_cast<SparklePacketStream *>(stream);
    _stream->addString(a);
}

//==================================================================================================

