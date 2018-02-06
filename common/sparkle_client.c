#include "sparkle_client.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "were/were_socket_unix.h"

//==================================================================================================

static void sparkle_client_data_handler(void *user);
static void sparkle_client_send(sparkle_client_t *client, sparkle_packet_t *packet);

//==================================================================================================

struct _sparkle_client_t
{
    were_socket_unix_t *socket;
    
    void (*data_callback)(sparkle_packet_t *packet, void *user);
    void *data_callback_user;
};

sparkle_client_t *sparkle_client_create(were_event_loop_t *loop)
{
    sparkle_client_t *client = (sparkle_client_t *)malloc(sizeof(sparkle_client_t));
    memset(client, 0x00, sizeof(sparkle_client_t));
    
    client->socket = were_socket_unix_create(loop);
    were_socket_unix_set_data_callback(client->socket, loop, sparkle_client_data_handler, client);

    return client;
}

void sparkle_client_destroy(sparkle_client_t *client)
{
    were_socket_unix_destroy(client->socket);
    
    free(client);
}

//==================================================================================================

void sparkle_client_set_connection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    were_socket_unix_set_connection_callback(client->socket, loop, f, user);
}

void sparkle_client_set_disconnection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    were_socket_unix_set_disconnection_callback(client->socket, loop, f, user);
}

void sparkle_client_set_data_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(sparkle_packet_t *packet, void *user), void *user)
{
    client->data_callback = f;
    client->data_callback_user = user;

}

//==================================================================================================

static void sparkle_client_data_handler(void *user)
{
    sparkle_client_t *client = (sparkle_client_t *)user;
    
    int bytesAvailable = were_socket_unix_bytes_available(client->socket);
    
    while (bytesAvailable >= sizeof(uint32_t))
    {
        uint32_t size = 0;
        were_socket_unix_peek(client->socket, &size, sizeof(uint32_t));
        
        if (bytesAvailable < sizeof(uint32_t) + size)
            return;
        
        were_socket_unix_receive(client->socket, &size, sizeof(uint32_t));
        bytesAvailable -= sizeof(uint32_t);
    
        void *data = malloc(size);
        were_socket_unix_receive(client->socket, data, size);
        bytesAvailable -= size;
    
        sparkle_packet_t *packet = sparkle_packet_create(data, size);
    
        if (client->data_callback != NULL)
            (*client->data_callback)(packet, client->data_callback_user);
    
        sparkle_packet_destroy(packet);
    
        free(data);
    }
}

//==================================================================================================

void sparkle_client_connect(sparkle_client_t *client, const char *path)
{
    were_socket_unix_connect(client->socket, path);
}

void sparkle_client_disconnect(sparkle_client_t *client)
{
    were_socket_unix_disconnect(client->socket);
}

static void sparkle_client_send(sparkle_client_t *client, sparkle_packet_t *packet)
{
    uint32_t size = sparkle_packet_position(packet);
    
    were_socket_unix_send(client->socket, &size, sizeof(uint32_t));
    
    were_socket_unix_send(client->socket, sparkle_packet_data(packet), size);
}

//==================================================================================================

void sparkle_client_register_surface_file(sparkle_client_t *client, const char *name, const char *path, int width, int height)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_REGISTER_SURFACE_FILE);
    sparkle_packet_add_string(packet, name);
    sparkle_packet_add_string(packet, path);
    sparkle_packet_add_uint32(packet, width);
    sparkle_packet_add_uint32(packet, height);
    sparkle_client_send(client, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_client_unregister_surface(sparkle_client_t *client, const char *name)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_UNREGISTER_SURFACE);
    sparkle_packet_add_string(packet, name);
    sparkle_client_send(client, packet);
    sparkle_packet_destroy(packet);
    
    usleep(100000);
}

void sparkle_client_set_surface_position(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_SET_SURFACE_POSITION);
    sparkle_packet_add_string(packet, name);
    sparkle_packet_add_uint32(packet, x1);
    sparkle_packet_add_uint32(packet, y1);
    sparkle_packet_add_uint32(packet, x2);
    sparkle_packet_add_uint32(packet, y2);
    sparkle_client_send(client, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_client_add_surface_damage(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_CLIENT_ADD_SURFACE_DAMAGE);
    sparkle_packet_add_string(packet, name);
    sparkle_packet_add_uint32(packet, x1);
    sparkle_packet_add_uint32(packet, y1);
    sparkle_packet_add_uint32(packet, x2);
    sparkle_packet_add_uint32(packet, y2);
    sparkle_client_send(client, packet);
    sparkle_packet_destroy(packet);
}

//==================================================================================================

