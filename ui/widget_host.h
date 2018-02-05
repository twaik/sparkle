#ifndef WIDGET_HOST_H
#define WIDGET_HOST_H

#include "sparkle.h"
#include "widget.h"

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
    WidgetHost(unsigned char *buffer, int stride, int width, int height);

    SizeA size();
    void *data();
    
    void addWidget(Widget *widget, const RectangleC &position);
    
    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    void pointerMotion(int slot, int x, int y);
    void keyDown(int code);
    void keyUp(int code);
    
    //FIXME Temporary
    void (*damageCallback)(int x1, int y1, int x2, int y2, void *user);
    void *damageCallbackUser;
    
private:
    void redrawWidget(Widget *widget);
    void transformCoordinates(int x, int y, WidgetData *widgetData, int *_x, int *_y);
    
private:
    uint32_t *_buffer;
    int _stride;
    int _width;
    int _height;

    std::vector<WidgetData> _widgets;
};

#endif //WIDGET_HOST_H

