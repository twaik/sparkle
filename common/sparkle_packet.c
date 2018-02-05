#include "sparkle_packet.h"
#include <stdlib.h>
#include <string.h>

//==================================================================================================

struct _sparkle_packet_t
{
    unsigned char *data;
    int allocated;
    int size;
    int position;
};

//==================================================================================================

sparkle_packet_t *sparkle_packet_create(unsigned char *data, int size)
{
    sparkle_packet_t *packet = (sparkle_packet_t *)malloc(sizeof(sparkle_packet_t));
    
    if (data == NULL)
    {
        packet->data = (unsigned char *)malloc(size);
        packet->allocated = 1;
    }
    else
    {
        packet->data = data;
        packet->allocated = 0;
    }
    
    packet->size = size;
    packet->position = 0;
    
    return packet;
}

//==================================================================================================

void sparkle_packet_destroy(sparkle_packet_t *packet)
{
    if (packet->allocated == 1)
    {
        free(packet->data);
    }
    
    free(packet);
}

//==================================================================================================

unsigned char *sparkle_packet_data(sparkle_packet_t *packet)
{
    return packet->data;
}

int sparkle_packet_position(sparkle_packet_t *packet)
{
    return packet->position;
}

//==================================================================================================

uint32_t sparkle_packet_get_uint32(sparkle_packet_t *packet)
{
    uint32_t *p = (uint32_t *)(packet->data + packet->position);
    packet->position += sizeof(uint32_t);

    return *p;
}

char *sparkle_packet_get_string(sparkle_packet_t *packet)
{
    char *p = (char *)(packet->data + packet->position);
    packet->position += strlen(p) + 1;
    
    return p;
}

void sparkle_packet_add_uint32(sparkle_packet_t *packet, uint32_t data)
{
    unsigned char *p = packet->data + packet->position;
    packet->position += sizeof(uint32_t);
    memcpy(p, &data, sizeof(uint32_t));
}

void sparkle_packet_add_string(sparkle_packet_t *packet, const char *data)
{
    unsigned char *p = packet->data + packet->position;
    int length = strlen(data) + 1;
    packet->position += length;
    memcpy(p, data, length);
}

//==================================================================================================

