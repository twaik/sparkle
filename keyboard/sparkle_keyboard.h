#ifndef SPARKLE_KEYBOARD_H
#define SPARKLE_KEYBOARD_H

#include "common/sparkle_packet.h"
#include <string>
#include <memory>

class WereEventLoop;
class WereSignalHandler;
class SparkleSurfaceFile;
class WereWidget;
class SparkleConnection;
class WereButton;

class SparkleKeyboard
{
public:
    ~SparkleKeyboard();
    SparkleKeyboard();
    
    void start();
    void reg();
    void unreg();
    void check();
    
private:
    void hostDamage(int x1, int y1, int x2, int y2);
    
    void packet(std::shared_ptr<SparklePacket> packet);
    
    void keyPressed(int code);
    void keyReleased(int code);
    
private:
    WereEventLoop *_loop;
    WereSignalHandler *_sig;
    SparkleSurfaceFile *_surface;
    WereWidget *_host;
    SparkleConnection *_sparkle;
    
    WereButton *_buttons;
    
    int _displayWidth;
    int _displayHeight;
    bool _show;
    bool _registered;
};

#endif //SPARKLE_KEYBOARD_H

