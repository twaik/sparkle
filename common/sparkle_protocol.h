#ifndef SPARKLE_PROTOCOL_H
#define SPARKLE_PROTOCOL_H

#include "sparkle_packet.h"

//==================================================================================================

struct _packet_type_t
{
    int code;
    packer_t packer;
};
typedef struct _packet_type_t packet_type_t;

//==================================================================================================

struct _register_surface_file_request {const char *name; const char *path; int width; int height;};
extern const packet_type_t register_surface_file_request;

struct _unregister_surface_request {const char *name;};
extern const packet_type_t unregister_surface_request;

struct _set_surface_position_request {const char *name; int x1; int y1; int x2; int y2;};
extern const packet_type_t set_surface_position_request;

struct _set_surface_strata_request {const char *name; int strata;};
extern const packet_type_t set_surface_strata_request;

struct _add_surface_damage_request {const char *name; int x1; int y1; int x2; int y2;};
extern const packet_type_t add_surface_damage_request;

struct _display_size_notification {int width; int height;};
extern const packet_type_t display_size_notification;

struct _pointer_down_notification {const char *surface; int slot; int x; int y;};
extern const packet_type_t pointer_down_notification;

struct _pointer_up_notification {const char *surface; int slot; int x; int y;};
extern const packet_type_t pointer_up_notification;

struct _pointer_motion_notification {const char *surface; int slot; int x; int y;};
extern const packet_type_t pointer_motion_notification;

struct _key_down_notification {int code;};
extern const packet_type_t key_down_notification;

struct _key_up_notification {int code;};
extern const packet_type_t key_up_notification;

struct _sound_data {unsigned int size; const unsigned char *data;};
extern const packet_type_t sound_data;
extern const packet_type_t sound_start;
extern const packet_type_t sound_stop;

//==================================================================================================

#endif //SPARKLE_PROTOCOL_H

