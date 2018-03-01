#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

//==================================================================================================

#ifdef __cplusplus
class SparklePacketStream;
#endif

struct _packer_t
{
#ifdef __cplusplus
    void (*pack)(SparklePacketStream *stream, void *data);
    void (*unpack)(SparklePacketStream *stream, void *data);
#else
    void *pack;
    void *unpack;
#endif
};
typedef struct _packer_t packer_t;

extern const packer_t p_uint32;
extern const packer_t p_string;

//==================================================================================================

struct SparklePacketHeader
{
    int operation;
};

//==================================================================================================
#ifdef __cplusplus

#include <vector>

class SparklePacket
{
public:
    ~SparklePacket();
    SparklePacket();
    
    unsigned char *data();
    unsigned int size() const;
    SparklePacketHeader *header();
    
    unsigned char *allocate(unsigned int size);
    void add(const unsigned char *bytes, unsigned int size);
    
private:
    std::vector<unsigned char> _data;
    SparklePacketHeader _header;
};

class SparklePacketStream
{
public:
    ~SparklePacketStream();
    SparklePacketStream(SparklePacket *packet);

    void addData(const unsigned char *a, unsigned int size);
    const unsigned char *getData(unsigned int size);
    
    void write(const void *data, unsigned int size);
    void read(void *data, unsigned int size);
    void pWrite(const packer_t *packer, void *data);
    void pRead(const packer_t *packer, void *data);
    
private:
    SparklePacket *_packet;
    unsigned int _readPosition;
    unsigned int _writePosition;
};

#endif
//==================================================================================================

typedef void sparkle_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_packet_t *sparkle_packet_create();
void sparkle_packet_destroy(sparkle_packet_t *packet);

unsigned char *sparkle_packet_data(sparkle_packet_t *packet);
unsigned int sparkle_packet_size(sparkle_packet_t *packet);
struct SparklePacketHeader *sparkle_packet_header(sparkle_packet_t *packet);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_PACKET_H

