#ifndef SPARKLE_SERVER_H
#define SPARKLE_SERVER_H

//==================================================================================================

#include "were/were.h"
#include "common/sparkle_packet.h"
#include "were/were_signal.h"
#include <set>
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
    
    void broadcast(SparklePacket *packet);

werethings:
    WereSignal<void (std::shared_ptr<SparkleConnection> client)> signal_connected;
    WereSignal<void (std::shared_ptr<SparkleConnection> client, std::shared_ptr<SparklePacket> packet)> signal_packet;
    
private:
    void handleConnection();
    void handleDisconnection(std::shared_ptr<SparkleConnection> client);
    void handlePacket(std::shared_ptr<SparkleConnection> client, std::shared_ptr<SparklePacket> packet);

private:
    WereEventLoop *_loop;
    WereServerUnix *_server;
    
    std::set< std::shared_ptr<SparkleConnection> > _clients;
};

//==================================================================================================

#endif //SPARKLE_SERVER_H

