#ifndef WIDGET_HOST_H
#define WIDGET_HOST_H

#include "sparkle.h"
#include "widget.h"

class WereEventLoop;

class WidgetData
{
public:
    Widget *_widget;
    RectangleC _position;
};

class WidgetHost
{
public:
    ~WidgetHost();
    WidgetHost(WereEventLoop *loop, unsigned char *buffer, int stride, int width, int height);

    int width();
    int height();
    void *data();
    
    void addWidget(Widget *widget, const RectangleC &position);
    
    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    void pointerMotion(int slot, int x, int y);
    void keyDown(int code);
    void keyUp(int code);
    
werethings:
    WereSignal<void (int x1, int y1, int x2, int y2)> damage;

    
private:
    void redrawWidget(Widget *widget);
    void transformCoordinates(int x, int y, WidgetData *widgetData, int *_x, int *_y);
    
private:
    WereEventLoop *_loop;
    unsigned char *_buffer;
    int _stride;
    int _width;
    int _height;

    std::vector<WidgetData> _widgets;
};

#endif //WIDGET_HOST_H

