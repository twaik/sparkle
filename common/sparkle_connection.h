#ifndef SPARKLE_CONNECTION_H
#define SPARKLE_CONNECTION_H

#include "common/sparkle_packet.h"
#include "sparkle_protocol.h" //FIXME

//==================================================================================================

#ifdef __cplusplus

#include "were/were.h"
#include "were/were_signal.h"
#include <string>
#include <memory>

class WereEventLoop;
class WereSocketUnix;
class WereTimer;

class SparkleConnection
{
public:
    ~SparkleConnection();
    SparkleConnection(WereEventLoop *loop, const std::string &path);
    SparkleConnection(WereEventLoop *loop, WereSocketUnix *socket);
    
    bool connected();
    
    void send(SparklePacket *packet);
    void send1(const SparklePacketType *packetType, void *data);
    static void unpack1(const SparklePacketType *packetType, SparklePacket *packet, void *data);

werethings:
    WereSignal<void ()> signal_connected;
    WereSignal<void ()> signal_disconnected;
    WereSignal<void (std::shared_ptr<SparklePacket> packet)> signal_packet;
    
private:
    void connect();
    void handleConnection();
    void handleDisconnection();
    void handleData();
    
private:
    WereEventLoop *_loop;
    std::string _path;
    WereSocketUnix *_socket;
    
    WereTimer *_connectTimer;
};

#else

struct SparkleConnection;
typedef struct SparkleConnection SparkleConnection;

#endif

//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

SparkleConnection *sparkle_connection_create(WereEventLoop *loop, const char *path);
void sparkle_connection_destroy(SparkleConnection *connection);

void sparkle_connection_send(SparkleConnection *connection, SparklePacket *packet);
void sparkle_connection_send1(SparkleConnection *connection, const SparklePacketType *packetType, void *data);
void sparkle_connection_unpack1(const SparklePacketType *packetType, SparklePacket *packet, void *data);

void sparkle_connection_add_connection_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user), void *user);
void sparkle_connection_add_disconnection_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user), void *user);
void sparkle_connection_add_packet_callback(SparkleConnection *connection, WereEventLoop *loop, void (*f)(void *user, SparklePacket *packet), void *user);


#ifdef __cplusplus
}
#endif

//==================================================================================================

#endif //SPARKLE_CONNECTION_H

