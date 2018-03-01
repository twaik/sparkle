#ifndef SPARKLE_KEYBOARD_H
#define SPARKLE_KEYBOARD_H

#include "common/sparkle_packet.h"
#include <string>
#include <memory>

class WereEventLoop;
class SparkleSurfaceFile;
class WidgetHost;
class SparkleConnection;
class WidgetButton;

class SparkleKeyboard
{
public:
    ~SparkleKeyboard();
    SparkleKeyboard();
    
    void start();
    void show();
    void hide();
    
private:
    void hostDamage(int x1, int y1, int x2, int y2);
    
    void packet(std::shared_ptr<SparklePacket> packet);
    
    void keyPressed(int code);
    void keyReleased(int code);
    
private:
    WereEventLoop *_loop;
    SparkleSurfaceFile *_surface;
    WidgetHost *_host;
    SparkleConnection *_sparkle;
    
    WidgetButton *_buttons;
    
    int _displayWidth;
    int _displayHeight;
};

#endif //SPARKLE_KEYBOARD_H

