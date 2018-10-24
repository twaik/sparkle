#ifndef SPARKLE_SERVER_H
#define SPARKLE_SERVER_H

#include "were/were.h"
#include "were/were_socket_unix.h"
#include "were/were_signal.h"
#include <set>
#include <string>
#include "sparkle_protocol.h" /* FIXME */

/* ================================================================================================================== */

class WereEventLoop;
class WereServerUnix;
class SparkleConnection;

class SparkleServer
{
public:
    ~SparkleServer();
    SparkleServer(WereEventLoop *loop, const std::string &path);

    void broadcast(WereSocketUnixMessage *message);
    void broadcast1(const SparklePacketType *packetType, void *data);

werethings:
    WereSignal<void (std::shared_ptr<SparkleConnection> client)> signal_connected;
    WereSignal<void (std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message)> signal_packet;

private:
    void handleConnection();
    void handleDisconnection(std::shared_ptr<SparkleConnection> client);
    void handlePacket(std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *_loop;
    WereServerUnix *_server;

    std::set< std::shared_ptr<SparkleConnection> > _clients;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SERVER_H */
