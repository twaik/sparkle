#ifndef SPARKLE_SERVER_H
#define SPARKLE_SERVER_H

//==================================================================================================

#include "common/sparkle_packet.h"
#include "were/were_function.h"
#include <vector>
#include <string>

//==================================================================================================

class WereEventLoop;
class WereServerUnix;
class SparkleConnection;

class SparkleServer
{
public:
    ~SparkleServer();
    SparkleServer(WereEventLoop *loop, const std::string &path);
    
    void broadcast(const SparklePacket &packet);

werethings:
    WereSignal<void (SparkleConnection *client)> signal_connected;
    WereSignal<void (SparkleConnection *client, SparklePacket packet)> signal_packet;
    
private:
    void handleConnection();
    void handleDisconnection(SparkleConnection *client);
    void handlePacket(SparkleConnection *client, SparklePacket packet);

private:
    WereEventLoop *_loop;
    WereServerUnix *_server;
    
    std::vector<SparkleConnection *> _clients;
};

//==================================================================================================

#endif //SPARKLE_SERVER_H

