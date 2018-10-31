#ifndef SPARKLE_SERVER_H
#define SPARKLE_SERVER_H

#include "were/were.h"
#include "were/were_signal.h"
#include "were/were_socket_unix.h"
#include "were/were_socket_unix_message_stream.h"
#include <set>
#include <string>

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

    template <typename T>
    void broadcast(const T &data)
    {
        WereSocketUnixMessage message;
        WereSocketUnixMessageStream stream(&message);
        stream << data;
        broadcast(&message);
    }

werethings:
    WereSignal<void (std::shared_ptr<SparkleConnection> client)> signal_connected;
    WereSignal<void (std::shared_ptr<SparkleConnection> client)> signal_disconnected;
    WereSignal<void (std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message)> signal_packet;

private:
    void handleConnection();
    void handleDisconnection(std::shared_ptr<SparkleConnection> client);
    void handleMessage(std::shared_ptr<SparkleConnection> client, std::shared_ptr<WereSocketUnixMessage> message);

private:
    WereEventLoop *_loop;
    WereServerUnix *_server;

    std::set< std::shared_ptr<SparkleConnection> > _clients;
};

/* ================================================================================================================== */

#endif /* SPARKLE_SERVER_H */
