#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

#include <stdint.h>

struct SparklePacketHeader
{
    int operation;
};

//==================================================================================================
#ifdef __cplusplus

#include <string>

class SparklePacket
{
public:
    ~SparklePacket();
    SparklePacket(unsigned int maxSize);
    
    unsigned char *data();
    unsigned int size() const;
    SparklePacketHeader *header();
    
    unsigned char *allocate(unsigned int size);
    void add(const unsigned char *bytes, unsigned int size);
    
private:
    unsigned int _maxSize;
    unsigned char *_data;
    unsigned int _size;
    SparklePacketHeader _header;
};

class SparklePacketStream
{
public:
    ~SparklePacketStream();
    SparklePacketStream(SparklePacket *packet);

    void addData(const unsigned char *a, unsigned int size);
    const unsigned char *getData(unsigned int size);
    
    void addUint32(uint32_t a);
    uint32_t getUint32();
    void addString(const std::string &a);
    std::string getString();
    const char *getStringPointer();
    
private:
    SparklePacket *_packet;
    unsigned int _readPosition;
    unsigned int _writePosition;
};

#endif
//==================================================================================================

typedef void sparkle_packet_t;
typedef void sparkle_packet_stream_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_packet_t *sparkle_packet_create(unsigned int size);
void sparkle_packet_destroy(sparkle_packet_t *packet);

unsigned char *sparkle_packet_data(sparkle_packet_t *packet);
unsigned int sparkle_packet_size(sparkle_packet_t *packet);
struct SparklePacketHeader *sparkle_packet_header(sparkle_packet_t *packet);

sparkle_packet_stream_t *sparkle_packet_stream_create(sparkle_packet_t *packet);
void sparkle_packet_stream_destroy(sparkle_packet_stream_t *stream);

uint32_t sparkle_packet_stream_get_uint32(sparkle_packet_stream_t *stream);
const char *sparkle_packet_stream_get_string(sparkle_packet_stream_t *stream);
void sparkle_packet_stream_add_uint32(sparkle_packet_stream_t *stream, uint32_t a);
void sparkle_packet_stream_add_string(sparkle_packet_stream_t *stream, const char *a);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_PACKET_H

