#ifndef WIDGET_H
#define WIDGET_H

#include "were/were_signal.h"

class Widget
{
public:
    virtual ~Widget() {}
    
    virtual void draw(unsigned char *buffer, int stride, int width, int height) = 0;
    
    WereSignal<void ()> damage;
    
    virtual void pointerDown(int slot, int x, int y) {}
    virtual void pointerUp(int slot, int x, int y) {}
    virtual void pointerMotion(int slot, int x, int y) {}
    virtual void keyDown(int code) {}
    virtual void keyUp(int code) {}
};

#endif //WIDGET_H

