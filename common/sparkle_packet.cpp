#include "sparkle_packet.h"
#include <string.h>

#include "were/were.h"

//==================================================================================================

SparklePacket::~SparklePacket()
{
}

SparklePacket::SparklePacket()
{
    _position = 0;
}

//==================================================================================================

const unsigned char *SparklePacket::data() const
{
    return _data.data();
}

unsigned int SparklePacket::size() const
{
    return _data.size();
}

unsigned int SparklePacket::readPosition() const
{
    return _position;
}

//==================================================================================================

void SparklePacket::addData(const unsigned char *a, unsigned int size)
{
    //were_debug("SparklePacket: Adding %d bytes.\n", size);
    _data.insert(_data.end(), a, a + size);
}

const unsigned char *SparklePacket::getData(unsigned int size)
{
    //were_debug("SparklePacket: Reading %d bytes.\n", size);
    const unsigned char *p = &_data[_position];
    _position += size;
    return p;
}

unsigned char *SparklePacket::allocate(unsigned int size)
{
    //were_debug("SparklePacket: Alloc %d bytes.\n", size);
    int start = _data.size();
    _data.resize(start + size);
    unsigned char *p = &_data[start];
    return p;
}

void SparklePacket::addUint32(uint32_t a)
{
    addData(reinterpret_cast<const unsigned char *>(&a), sizeof(uint32_t));
}

uint32_t SparklePacket::getUint32()
{
    const uint32_t *p = reinterpret_cast<const uint32_t *>(getData(sizeof(uint32_t)));
    return *p;
}

void SparklePacket::addString(const std::string &a)
{
    addUint32(a.length());
    addData(reinterpret_cast<const unsigned char *>(a.c_str()), a.length() + 1);
}

std::string SparklePacket::getString()
{
    uint32_t length = getUint32();
    const char *p = reinterpret_cast<const char*>(getData(length + 1));
    return std::string(p, length);
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

const unsigned char *sparkle_packet_data(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->data();
}

unsigned int sparkle_packet_size(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->size();
}

uint32_t sparkle_packet_get_uint32(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    return _packet->getUint32();
}

const char *sparkle_packet_get_string(sparkle_packet_t *packet)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    uint32_t length = _packet->getUint32();
    return reinterpret_cast<const char *>(_packet->getData(length + 1));
}

void sparkle_packet_add_uint32(sparkle_packet_t *packet, uint32_t a)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    _packet->addUint32(a);
}

void sparkle_packet_add_string(sparkle_packet_t *packet, const char *a)
{
    SparklePacket *_packet = static_cast<SparklePacket *>(packet);
    _packet->addString(a);
}

//==================================================================================================

