#ifndef WIDGET_H
#define WIDGET_H

#include "sparkle.h"

class Widget
{
public:
    virtual ~Widget() {}
    
    virtual void draw(uint32_t *buffer, int stride, int width, int height) = 0;
    
    WFunctionL<void (Widget *widget)> damage;
    
    virtual void pointerDown(int slot, int x, int y) {}
    virtual void pointerUp(int slot, int x, int y) {}
    virtual void pointerMotion(int slot, int x, int y) {}
    virtual void keyDown(int code) {}
    virtual void keyUp(int code) {}
};

#endif //WIDGET_H

