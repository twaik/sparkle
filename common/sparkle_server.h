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
class WereSocketUnix;

class SparkleServer
{
public:
    ~SparkleServer();
    SparkleServer(WereEventLoop *loop, const std::string &path);
    
    void displaySize(int width, int height);
    void pointerDown(const std::string &surface, int slot, int x, int y);
    void pointerUp(const std::string &surface, int slot, int x, int y);
    void pointerMotion(const std::string &surface, int slot, int x, int y);
    void keyDown(int code);
    void keyUp(int code);
    
werethings:
    WereSignal<void ()> connection;
    WereSignal<void (const std::string &name, const std::string &path, int width, int height)> registerSurfaceFile;
    WereSignal<void (const std::string &name)> unregisterSurface;
    WereSignal<void (const std::string &name, int x1, int y1, int x2, int y2)> setSurfacePosition;
    WereSignal<void (const std::string &name, int x1, int y1, int x2, int y2)> addSurfaceDamage;
    
private:
    void handleConnection();
    void handleDisconnection(WereSocketUnix *client);
    void handleData(WereSocketUnix *client);
    void handlePacket(sparkle_packet_t *packet);
    void broadcast(sparkle_packet_t *packet);
    
private:
    WereEventLoop *_loop;
    WereServerUnix *_server;
    
    std::vector<WereSocketUnix *> _clients;
};

//==================================================================================================

#endif //SPARKLE_SERVER_H

