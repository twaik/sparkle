#ifndef SPARKLE_CLIENT_H
#define SPARKLE_CLIENT_H

#include <stdint.h>
#include "were/were_event_loop.h"
#include "common/sparkle_packet.h"
#include "common/sparkle_protocol.h"

//==================================================================================================

struct _sparkle_client_t;
typedef struct _sparkle_client_t sparkle_client_t;

//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

sparkle_client_t *sparkle_client_create(were_event_loop_t *loop);
void sparkle_client_destroy(sparkle_client_t *client);

void sparkle_client_set_connection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user);
void sparkle_client_set_disconnection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user);
void sparkle_client_set_data_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(sparkle_packet_t *packet, void *user), void *user);

void sparkle_client_connect(sparkle_client_t *client, const char *path);
void sparkle_client_disconnect(sparkle_client_t *client);


void sparkle_client_register_surface_file(sparkle_client_t *client, const char *name, const char *path, int width, int height);
void sparkle_client_unregister_surface(sparkle_client_t *client, const char *name);
void sparkle_client_set_surface_position(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2);
void sparkle_client_add_surface_damage(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_CLIENT_H

