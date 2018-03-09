#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

//==================================================================================================

struct SparklePacketHeader
{
    int operation;
};

//==================================================================================================

#ifdef __cplusplus

#include <vector>

struct WerePacker; //FIXME

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
    void pWrite(const WerePacker *packer, void *data);
    void pRead(const WerePacker *packer, void *data);
    
private:
    SparklePacket *_packet;
    unsigned int _readPosition;
    unsigned int _writePosition;
};

#else

struct SparklePacket;
typedef struct SparklePacket SparklePacket;

struct SparklePacketStream;
typedef struct SparklePacketStream SparklePacketStream;

#endif

//==================================================================================================

struct WerePacker
{
    void (*pack)(SparklePacketStream *stream, void *data);
    void (*unpack)(SparklePacketStream *stream, void *data);
};
typedef struct WerePacker WerePacker;

extern const WerePacker p_uint32;
extern const WerePacker p_string;
extern const WerePacker p_float;

//==================================================================================================


#ifdef __cplusplus
extern "C" {
#endif

SparklePacket *sparkle_packet_create();
void sparkle_packet_destroy(SparklePacket *packet);

unsigned char *sparkle_packet_data(SparklePacket *packet);
unsigned int sparkle_packet_size(SparklePacket *packet);
struct SparklePacketHeader *sparkle_packet_header(SparklePacket *packet);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_PACKET_H

