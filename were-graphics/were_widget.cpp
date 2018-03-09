#include "were_widget.h"
#include "were_surface.h"

WereWidget::~WereWidget()
{
}

WereWidget::WereWidget()
{
    _parent = 0;
    _surface = 0;
}

WereSurface *WereWidget::surface()
{
    if (_surface)
        return _surface;
    else if (_parent)
        return _parent->surface();
    else
        return 0;
}

int WereWidget::x()
{
    if (_surface)
        return 0;
    else if (_parent)
        return _parent->childX(this);
    else
        return 0; //FIXME
}

int WereWidget::y()
{
    if (_surface)
        return 0;
    else if (_parent)
        return _parent->childY(this);
    else
        return 0; //FIXME
}

int WereWidget::width()
{
    if (_surface)
        return _surface->width();
    else if (_parent)
        return _parent->childWidth(this);
    else
        return 0;
}

int WereWidget::height()
{
    if (_surface)
        return _surface->height();
    else if (_parent)
        return _parent->childHeight(this);
    else
        return 0;
}

void WereWidget::setParent(WereWidget *widget)
{
    _parent = widget;
}

void WereWidget::setSurface(WereSurface *surface)
{
    _surface = surface;
    draw();
}

void WereWidget::addChild(WereWidget *widget, const RectangleC &position)
{
    _children[widget] = position;
    widget->setParent(this);
}

int WereWidget::childX(WereWidget *widget)
{
    auto it = _children.find(widget);
    if (it == _children.end())
        return 0; //FIXME

    int x1 = x() + it->second.from.x.relative * width() + it->second.from.x.absolute;
    
    return x1;
}

int WereWidget::childY(WereWidget *widget)
{
    auto it = _children.find(widget);
    if (it == _children.end())
        return 0; //FIXME

    int y1 = y() + it->second.from.y.relative * height() + it->second.from.y.absolute;
    
    return y1;
}

int WereWidget::childWidth(WereWidget *widget)
{
    auto it = _children.find(widget);
    if (it == _children.end())
        return 0; //FIXME
        
    int x1 = x() + it->second.from.x.relative * width() + it->second.from.x.absolute;
    int x2 = x() + it->second.to.x.relative * width() + it->second.to.x.absolute;
    
    return x2 - x1;
}

int WereWidget::childHeight(WereWidget *widget)
{
    auto it = _children.find(widget);
    if (it == _children.end())
        return 0; //FIXME
        
    int y1 = y() + it->second.from.y.relative * height() + it->second.from.y.absolute;
    int y2 = y() + it->second.to.y.relative * height() + it->second.to.y.absolute;
    
    return y2 - y1;
}

void WereWidget::draw()
{
    for (auto it = _children.begin(); it != _children.end(); ++it)
        it->first->draw();
}

void WereWidget::pointerDown(int slot, int x, int y)
{
    for (auto it = _children.rbegin(); it != _children.rend(); ++it)
    {
        int _x;
        int _y;
        transformCoordinates(x, y, it->second, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            it->first->pointerDown(slot, _x, _y);
            return;
        }
    }
}

void WereWidget::pointerUp(int slot, int x, int y)
{
    for (auto it = _children.rbegin(); it != _children.rend(); ++it)
    {
        int _x;
        int _y;
        transformCoordinates(x, y, it->second, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            it->first->pointerUp(slot, _x, _y);
            return;
        }
    }
}

void WereWidget::pointerMotion(int slot, int x, int y)
{
    for (auto it = _children.rbegin(); it != _children.rend(); ++it)
    {
        int _x;
        int _y;
        transformCoordinates(x, y, it->second, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            it->first->pointerMotion(slot, _x, _y);
            return;
        }
    }
}

void WereWidget::keyDown(int code)
{
}

void WereWidget::keyUp(int code)
{
}

void WereWidget::transformCoordinates(int x, int y, const RectangleC &position, int *_x, int *_y)
{
    *_x = -1;
    *_y = -1;
    
    int x1a = position.from.x.relative * width() + position.from.x.absolute;
    int y1a = position.from.y.relative * height() + position.from.y.absolute;
    int x2a = position.to.x.relative * width() + position.to.x.absolute;
    int y2a = position.to.y.relative * height() + position.to.y.absolute;
    
    if (x < x1a || x > x2a || y < y1a || y > y2a)
        return;
    
    *_x = (x - x1a);
    *_y = (y - y1a);
}

