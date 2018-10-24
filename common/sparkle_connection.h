#ifndef SPARKLE_CONNECTION_H
#define SPARKLE_CONNECTION_H

#include "were/were.h"
#include "were/were_signal.h"
#include "were/were_socket_unix.h"
#include <string>
#include <memory>
#include "sparkle_protocol.h" /* FIXME */

/* ================================================================================================================== */

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

    void send(WereSocketUnixMessage *message);
    void send1(const SparklePacketType *packetType, void *data);

werethings:
    WereSignal<void ()> signal_connected;
    WereSignal<void ()> signal_disconnected;
    WereSignal<void (std::shared_ptr<WereSocketUnixMessage> message)> signal_packet;

private:
    void connect();
    void handleConnection();
    void handleDisconnection();
    void handleMessage(std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *_loop;
    std::string _path;
    WereSocketUnix *_socket;

    WereTimer *_connectTimer;
};

/* ================================================================================================================== */

#endif /* SPARKLE_CONNECTION_H */
