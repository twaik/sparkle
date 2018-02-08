#ifndef SPARKLE_KEYBOARD_H
#define SPARKLE_KEYBOARD_H

#include <string>

class WereEventLoop;
class SparkleSurfaceFile;
class WidgetHost;
class SparkleClient;
class WidgetButton;

class SparkleKeyboard
{
public:
    ~SparkleKeyboard();
    SparkleKeyboard();
    
    void start();
    
private:
    void handleConnection();
    void hostDamage(int x1, int y1, int x2, int y2);
    
    void pointerDown(const std::string &name, int slot, int x, int y);
    void pointerUp(const std::string &name, int slot, int x, int y);
    
private:
    WereEventLoop *_loop;
    SparkleSurfaceFile *_surface;
    WidgetHost *_host;
    SparkleClient *_client;
    
    WidgetButton *_buttons;
};

#endif //SPARKLE_KEYBOARD_H

