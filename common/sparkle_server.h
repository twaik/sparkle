#ifndef SPARKLE_SERVER_H
#define SPARKLE_SERVER_H

#include <stdint.h>
#include "were/were_event_loop.h"
#include "common/sparkle_packet.h"
#include "common/sparkle_protocol.h"

//==================================================================================================

struct _sparkle_server_t;
typedef struct _sparkle_server_t sparkle_server_t;

struct _sparkle_server_client_t;
typedef struct _sparkle_server_client_t sparkle_server_client_t;

//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

sparkle_server_t *sparkle_server_create(were_event_loop_t *loop, const char *path);
void sparkle_server_destroy(sparkle_server_t *server);

void sparkle_server_set_connection_callback(sparkle_server_t *server, were_event_loop_t *loop, void (*f)(void *user), void *user);
void sparkle_server_set_data_callback(sparkle_server_t *server, were_event_loop_t *loop, void (*f)(sparkle_packet_t *packet, void *user), void *user);


void sparkle_server_display_size(sparkle_server_t *server, int width, int height);
void sparkle_server_pointer_down(sparkle_server_t *server, const char *surface, int slot, int x, int y);
void sparkle_server_pointer_up(sparkle_server_t *server, const char *surface, int slot, int x, int y);
void sparkle_server_pointer_motion(sparkle_server_t *server, const char *surface, int slot, int x, int y);
void sparkle_server_key_down(sparkle_server_t *server, int code);
void sparkle_server_key_up(sparkle_server_t *server, int code);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_SERVER_H

