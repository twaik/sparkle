#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

#include <stdint.h>

//==================================================================================================
#ifdef __cplusplus

#include <string>

class SparklePacket
{
public:
    ~SparklePacket();
    SparklePacket(unsigned int size);
    
    unsigned char *data();
    unsigned int size() const;
    
private:
    unsigned char *_data;
    unsigned int _size;
};

class SparklePacketStream
{
public:
    ~SparklePacketStream();
    SparklePacketStream(unsigned char *data, unsigned int size);
    
    unsigned int writePosition() const;
    unsigned int readPosition() const;
    
    void addData(const unsigned char *a, unsigned int size);
    const unsigned char *getData(unsigned int size);
    
    void addUint32(uint32_t a);
    uint32_t getUint32();
    void addString(const std::string &a);
    std::string getString();
    
private:
    unsigned char *_data;
    unsigned int _size;
    unsigned int _writePosition;
    unsigned int _readPosition;
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

sparkle_packet_stream_t *sparkle_packet_stream_create(unsigned char *data, unsigned int size);
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

