#ifndef SPARKLE_PROTOCOL_H
#define SPARKLE_PROTOCOL_H

#include "were/were_stream.h"

/* ================================================================================================================== */

typedef struct
{
    int code;
    WerePacker packer;
} SparklePacketType;

/* ================================================================================================================== */

struct register_surface_file_request_ {const char *name; const char *path; int width; int height;};
extern const SparklePacketType register_surface_file_request;

struct unregister_surface_request_ {const char *name;};
extern const SparklePacketType unregister_surface_request;

struct set_surface_position_request_ {const char *name; int x1; int y1; int x2; int y2;};
extern const SparklePacketType set_surface_position_request;

struct set_surface_strata_request_ {const char *name; int strata;};
extern const SparklePacketType set_surface_strata_request;

struct set_surface_alpha_request_ {const char *name; float alpha;};
extern const SparklePacketType set_surface_alpha_request;

struct add_surface_damage_request_ {const char *name; int x1; int y1; int x2; int y2;};
extern const SparklePacketType add_surface_damage_request;

struct display_size_notification_ {int width; int height;};
extern const SparklePacketType display_size_notification;

struct pointer_down_notification_ {const char *surface; int slot; int x; int y;};
extern const SparklePacketType pointer_down_notification;

struct pointer_up_notification_ {const char *surface; int slot; int x; int y;};
extern const SparklePacketType pointer_up_notification;

struct pointer_motion_notification_ {const char *surface; int slot; int x; int y;};
extern const SparklePacketType pointer_motion_notification;

struct key_down_notification_ {int code;};
extern const SparklePacketType key_down_notification;

struct key_up_notification_ {int code;};
extern const SparklePacketType key_up_notification;

struct sound_data_ {unsigned int size; const unsigned char *data;};
extern const SparklePacketType sound_data;

extern const SparklePacketType sound_start;

extern const SparklePacketType sound_stop;

struct key_down_request_ {int code;};
extern const SparklePacketType key_down_request;

struct key_up_request_ {int code;};
extern const SparklePacketType key_up_request;

/* ================================================================================================================== */

#endif /* SPARKLE_PROTOCOL_H */
