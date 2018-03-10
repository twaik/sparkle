#ifndef WERE_WIDGET_H
#define WERE_WIDGET_H

#include "were/were.h"
#include "utility.h"
#include <map>

class WereSurface;

class WereWidget
{
public:
    virtual ~WereWidget();
    WereWidget();
    
    WereSurface *surface();
    int x();
    int y();
    int width();
    int height();
    
    void setParent(WereWidget *widget);
    void setSurface(WereSurface *surface);
    
    void addChild(WereWidget *widget, const RectangleC &position);
    int childX(WereWidget *widget);
    int childY(WereWidget *widget);
    int childWidth(WereWidget *widget);
    int childHeight(WereWidget *widget);
    
    virtual void draw();
    
    virtual void pointerDown(int slot, int x, int y);
    virtual void pointerUp(int slot, int x, int y);
    virtual void pointerMotion(int slot, int x, int y);
    virtual void keyDown(int code);
    virtual void keyUp(int code);
    virtual void pointerEnter(int slot);
    virtual void pointerLeave(int slot);
    WereWidget *widgetAt(int x, int y, int *wX, int *wY);
    
private:
    
private:
    WereWidget *_parent;
    WereSurface *_surface;
    
    std::map<WereWidget *, RectangleC> _children;
    std::array<WereWidget *, 10> _underPointer;
};

#endif //WERE_WIDGET_H

