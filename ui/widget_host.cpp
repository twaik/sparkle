#include "widget_host.h"

WidgetHost::~WidgetHost()
{
}

WidgetHost::WidgetHost(unsigned char *buffer, int stride, int width, int height)
{
    _buffer = reinterpret_cast<uint32_t *>(buffer);
    _stride = stride;
    _width = width;
    _height = height;
}

SizeA WidgetHost::size()
{
    return SizeA(_width, _height);
}
    
void *WidgetHost::data()
{
    return _buffer;
}

void WidgetHost::addWidget(Widget *widget, const RectangleC &position)
{
    WidgetData widgetData;
    widgetData._widget = widget;
    widgetData._position = position;
    _widgets.push_back(widgetData);

    widget->damage.connect(this, &WidgetHost::redrawWidget);    
    redrawWidget(widgetData._widget);
}

void WidgetHost::redrawWidget(Widget *widget)
{
    WidgetData *widgetData = 0;
    
    for (unsigned int i = 0; i < _widgets.size(); ++i)
    {
        if (_widgets[i]._widget == widget)
        {
            widgetData = &_widgets[i];
        }
    }
    
    if (widgetData == 0)
    {
        return;
    }
    
    int x1 = widgetData->_position.from.x.relative * _width + widgetData->_position.from.x.absolute;
    int y1 = widgetData->_position.from.y.relative * _height + widgetData->_position.from.y.absolute;
    int x2 = widgetData->_position.to.x.relative * _width + widgetData->_position.to.x.absolute;
    int y2 = widgetData->_position.to.y.relative * _height + widgetData->_position.to.y.absolute;
    int width = x2 - x1;
    int height = y2 - y1;
        

    widget->draw(&_buffer[y1 * _width + x1], _width, width, height);
    
    //damage(this, RectangleA(PointA(x1, y1), PointA(x2, y2)));
    
    (*damageCallback)(x1, y1, x2, y2, damageCallbackUser);

}

void WidgetHost::pointerDown(int slot, int x, int y)
{
    for (unsigned int i = _widgets.size(); i > 0; --i)
    {
        WidgetData *widgetData = &_widgets[i - 1];
        Widget *widget = widgetData->_widget;
        int _x;
        int _y;
        transformCoordinates(x, y, widgetData, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            widget->pointerDown(slot, _x, _y);
            return;
        }
    }
}

void WidgetHost::pointerUp(int slot, int x, int y)
{
    for (unsigned int i = _widgets.size(); i > 0; --i)
    {
        WidgetData *widgetData = &_widgets[i - 1];
        Widget *widget = widgetData->_widget;
        int _x;
        int _y;
        transformCoordinates(x, y, widgetData, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            widget->pointerUp(slot, _x, _y);
            return;
        }
    }
}

void WidgetHost::pointerMotion(int slot, int x, int y)
{
    for (unsigned int i = _widgets.size(); i > 0; --i)
    {
        WidgetData *widgetData = &_widgets[i - 1];
        Widget *widget = widgetData->_widget;
        int _x;
        int _y;
        transformCoordinates(x, y, widgetData, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            widget->pointerMotion(slot, _x, _y);
            return;
        }
    }
}

void WidgetHost::keyDown(int code)
{
}

void WidgetHost::keyUp(int code)
{
}

void WidgetHost::transformCoordinates(int x, int y, WidgetData *widgetData, int *_x, int *_y)
{
    *_x = -1;
    *_y = -1;
    
    int x1a = widgetData->_position.from.x.relative * _width + widgetData->_position.from.x.absolute;
    int y1a = widgetData->_position.from.y.relative * _height + widgetData->_position.from.y.absolute;
    int x2a = widgetData->_position.to.x.relative * _width + widgetData->_position.to.x.absolute;
    int y2a = widgetData->_position.to.y.relative * _height + widgetData->_position.to.y.absolute;
    
    if (x < x1a || x > x2a || y < y1a || y > y2a)
    {
        return;
    }
    
    *_x = (x - x1a);
    *_y = (y - y1a);
}

