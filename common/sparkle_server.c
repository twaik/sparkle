#include "sparkle_server.h"
#include <stdlib.h>
#include <string.h>

#include "were/were_server_unix.h"
#include "were/were_socket_unix.h"

//==================================================================================================

#define MAX_CLIENTS 16

//==================================================================================================

static void sparkle_server_connection(void *user);
static void sparkle_server_data(void *user);
static void sparkle_server_send(sparkle_server_t *server, sparkle_packet_t *packet);

//==================================================================================================

struct _sparkle_server_client_t
{
    sparkle_server_t *server;
    were_socket_unix_t *socket;
};

struct _sparkle_server_t
{
    were_event_loop_t *loop;
    were_server_unix_t *server;
    
    void (*data_callback)(sparkle_packet_t *packet, void *user);
    void *data_callback_user;
    
    sparkle_server_client_t clients[MAX_CLIENTS];
};

sparkle_server_t *sparkle_server_create(were_event_loop_t *loop, const char *path)
{
    sparkle_server_t *server = (sparkle_server_t *)malloc(sizeof(sparkle_server_t));
    memset(server, 0x00, sizeof(sparkle_server_t));
    
    server->loop = loop;
    
    server->server = were_server_unix_create(loop, path);
    were_server_unix_set_connection_callback(server->server, loop, sparkle_server_connection, server);

    return server;
}

void sparkle_server_destroy(sparkle_server_t *server)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        were_socket_unix_t *client = server->clients[i].socket;
        were_socket_unix_destroy(client);
    }
    
    were_server_unix_destroy(server->server);
    
    free(server);
}

//==================================================================================================

static void sparkle_server_connection(void *user)
{
    sparkle_server_t *server = (sparkle_server_t *)user;
    
    were_socket_unix_t *client = were_server_unix_accept(server->server);
    if (client == NULL)
        return;
    
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (server->clients[i].socket == NULL)
        {
            server->clients[i].server = server;
            server->clients[i].socket = client;
            were_socket_unix_set_data_callback(client, server->loop, sparkle_server_data, &server->clients[i]);
            return;
        }
    }
    
    were_socket_unix_destroy(client);
}

static void sparkle_server_data(void *user)
{
    sparkle_server_client_t *client = (sparkle_server_client_t *)user;

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
    
        sparkle_packet_t *packet = sparkle_packet_create((unsigned char *)data, size);
    
        if (client->server->data_callback != NULL)
            (*client->server->data_callback)(packet, client->server->data_callback_user);
    
        sparkle_packet_destroy(packet);
    
        free(data);
    }
}

//==================================================================================================

void sparkle_server_set_connection_callback(sparkle_server_t *server, were_event_loop_t *loop, void (*f)(void *user), void *user)
{
    were_server_unix_set_connection_callback(server->server, loop, f, user);
}

void sparkle_server_set_data_callback(sparkle_server_t *server, were_event_loop_t *loop, void (*f)(sparkle_packet_t *packet, void *user), void *user)
{
    server->data_callback = f;
    server->data_callback_user = user;
}

//==================================================================================================

static void sparkle_server_send(sparkle_server_t *server, sparkle_packet_t *packet)
{
    uint32_t size = sparkle_packet_position(packet);
    
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        were_socket_unix_t *client = server->clients[i].socket;
        if (client == NULL)
            continue;
        
        if (were_socket_unix_connected(client) == 0)
        {
            were_socket_unix_destroy(client);
            server->clients[i].socket = NULL;
        }
        else
        {
            were_socket_unix_send(client, &size, sizeof(uint32_t));
            were_socket_unix_send(client, sparkle_packet_data(packet), size);
        }
    }
}

//==================================================================================================

void sparkle_server_display_size(sparkle_server_t *server, int width, int height)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_DISPLAY_SIZE);
    sparkle_packet_add_uint32(packet, width);
    sparkle_packet_add_uint32(packet, height);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_server_pointer_down(sparkle_server_t *server, const char *surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_DOWN);
    sparkle_packet_add_string(packet, surface);
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_server_pointer_up(sparkle_server_t *server, const char *surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_UP);
    sparkle_packet_add_string(packet, surface);
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_server_pointer_motion(sparkle_server_t *server, const char *surface, int slot, int x, int y)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_POINTER_MOTION);
    sparkle_packet_add_string(packet, surface);
    sparkle_packet_add_uint32(packet, slot);
    sparkle_packet_add_uint32(packet, x);
    sparkle_packet_add_uint32(packet, y);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_server_key_down(sparkle_server_t *server, int code)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_KEY_DOWN);
    sparkle_packet_add_uint32(packet, code);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

void sparkle_server_key_up(sparkle_server_t *server, int code)
{
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SERVER_KEY_UP);
    sparkle_packet_add_uint32(packet, code);
    sparkle_server_send(server, packet);
    sparkle_packet_destroy(packet);
}

//==================================================================================================

