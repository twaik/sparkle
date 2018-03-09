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
    
private:
    void transformCoordinates(int x, int y, const RectangleC &position, int *_x, int *_y);
    
private:
    WereWidget *_parent;
    WereSurface *_surface;
    
    std::map<WereWidget *, RectangleC> _children;
};

#endif //WERE_WIDGET_H

