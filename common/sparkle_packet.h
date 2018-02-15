#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

#include <stdint.h>

//==================================================================================================
#ifdef __cplusplus

#include <string>
#include <vector>

class SparklePacket
{
public:
    ~SparklePacket();
    SparklePacket();
    
    const unsigned char *data() const;
    unsigned int size() const;
    unsigned int readPosition() const;
    
    void addData(const unsigned char *a, unsigned int size);
    const unsigned char *getData(unsigned int size);
    unsigned char *allocate(unsigned int size);
    
    void addUint32(uint32_t a);
    uint32_t getUint32();
    void addString(const std::string &a);
    std::string getString();
    
private:
    std::vector<unsigned char> _data;
    int _position;
};

#endif
//==================================================================================================

typedef void sparkle_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_packet_t *sparkle_packet_create();
void sparkle_packet_destroy(sparkle_packet_t *packet);

const unsigned char *sparkle_packet_data(sparkle_packet_t *packet);
unsigned int sparkle_packet_size(sparkle_packet_t *packet);

uint32_t sparkle_packet_get_uint32(sparkle_packet_t *packet);
const char *sparkle_packet_get_string(sparkle_packet_t *packet);
void sparkle_packet_add_uint32(sparkle_packet_t *packet, uint32_t a);
void sparkle_packet_add_string(sparkle_packet_t *packet, const char *a);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_PACKET_H

