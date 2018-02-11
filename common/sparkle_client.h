#ifndef SPARKLE_CLIENT_H
#define SPARKLE_CLIENT_H

#include <stdint.h>
#include "were/were_event_loop.h"
#include "common/sparkle_packet.h"
#include "common/sparkle_protocol.h"

//==================================================================================================

#ifdef __cplusplus

#include "were/were_function.h"

class WereSocketUnix;
class WereTimer;

class SparkleClient
{
public:
    ~SparkleClient();
    SparkleClient(WereEventLoop *loop, const std::string &path);
    
    WereSocketUnix *socket();
    
    void connect();
    void disconnect();
    
    void registerSurfaceFile(const std::string &name, const std::string &path, int width, int height);
    void unregisterSurface(const std::string &name);
    void setSurfacePosition(const std::string &name, int x1, int y1, int x2, int y2);
    void setSurfaceStrata(const std::string &name, int strata);
    void addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2);
    void keyPress(int code);
    void keyRelease(int code);
    
    
werethings:
    WereSignal<void ()> connection;
    WereSignal<void ()> disconnection;
    WereSignal<void (int width, int height)> displaySize;
    WereSignal<void (const std::string &name, int slot, int x, int y)> pointerDown;
    WereSignal<void (const std::string &name, int slot, int x, int y)> pointerUp;
    WereSignal<void (const std::string &name, int slot, int x, int y)> pointerMotion;
    WereSignal<void (int code)> keyDown;
    WereSignal<void (int code)> keyUp;
    
private:
    void readData();
    void handlePacket(sparkle_packet_t *packet);
    void send(sparkle_packet_t *packet);
    void connectTimeout();
    
private:
    WereEventLoop *_loop;
    std::string _path;
    WereSocketUnix *_socket;
    
    WereTimer *_connectTimer;
};

#endif

//==================================================================================================

typedef void sparkle_client_t;

#ifdef __cplusplus
extern "C" {
#endif

sparkle_client_t *sparkle_client_create(were_event_loop_t *loop, const char *path);
void sparkle_client_destroy(sparkle_client_t *client);

void sparkle_client_set_connection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user);
void sparkle_client_set_disconnection_callback(sparkle_client_t *client, were_event_loop_t *loop, void (*f)(void *user), void *user);
void sparkle_client_set_operation_callback(sparkle_client_t *client, were_event_loop_t *loop, int operation, void *f, void *user);

void sparkle_client_connect(sparkle_client_t *client);
void sparkle_client_disconnect(sparkle_client_t *client);


void sparkle_client_register_surface_file(sparkle_client_t *client, const char *name, const char *path, int width, int height);
void sparkle_client_unregister_surface(sparkle_client_t *client, const char *name);
void sparkle_client_set_surface_position(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2);
void sparkle_client_set_surface_strata(sparkle_client_t *client, const char *name, int strata);
void sparkle_client_add_surface_damage(sparkle_client_t *client, const char *name, int x1, int y1, int x2, int y2);

#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_CLIENT_H

