#ifndef SPARKLE_PACKET_H
#define SPARKLE_PACKET_H

#include <stdint.h>

struct _sparkle_packet_t;
typedef struct _sparkle_packet_t sparkle_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_packet_t *sparkle_packet_create(unsigned char *data, int size);
void sparkle_packet_destroy(sparkle_packet_t *packet);

unsigned char *sparkle_packet_data(sparkle_packet_t *packet);
int sparkle_packet_position(sparkle_packet_t *packet);

uint32_t sparkle_packet_get_uint32(sparkle_packet_t *packet);
char *sparkle_packet_get_string(sparkle_packet_t *packet);

void sparkle_packet_add_uint32(sparkle_packet_t *packet, uint32_t data);
void sparkle_packet_add_string(sparkle_packet_t *packet, const char *data);

#ifdef __cplusplus
}
#endif

#endif //SPARKLE_PACKET_H

