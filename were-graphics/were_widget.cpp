#include "were_widget.h"
#include "were_surface.h"

WereWidget::~WereWidget()
{
}

WereWidget::WereWidget()
{
    _parent = nullptr;
    _surface = nullptr;
    _underPointer = {};
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
    int _x, _y;
    WereWidget *widget = widgetAt(x, y, &_x, &_y);
    if (widget != nullptr)
        widget->pointerDown(slot, _x, _y);
}

void WereWidget::pointerUp(int slot, int x, int y)
{
    int _x, _y;
    WereWidget *widget = widgetAt(x, y, &_x, &_y);
    if (widget != nullptr)
        widget->pointerUp(slot, _x, _y);
}

void WereWidget::pointerMotion(int slot, int x, int y)
{
    int _x, _y;
    WereWidget *widget = widgetAt(x, y, &_x, &_y);
    
    WereWidget *before = _underPointer[slot];
    
    if (widget != before)
    {
        if (before != nullptr)
            before->pointerLeave(slot);
        
        if (widget != nullptr)
            widget->pointerEnter(slot);
            
        _underPointer[slot] = widget;
    }
    
    if (widget != nullptr)
        widget->pointerMotion(slot, _x, _y);
}

void WereWidget::keyDown(int code)
{
}

void WereWidget::keyUp(int code)
{
}

void WereWidget::pointerEnter(int slot)
{
}

void WereWidget::pointerLeave(int slot)
{
    WereWidget *before = _underPointer[slot];
    if (before != nullptr)
        before->pointerLeave(slot);
}

WereWidget *WereWidget::widgetAt(int x, int y, int *wX, int *wY)
{
    for (auto it = _children.begin(); it != _children.end(); ++it)
    {
        int x1a = it->second.from.x.relative * width() + it->second.from.x.absolute;
        int y1a = it->second.from.y.relative * height() + it->second.from.y.absolute;
        int x2a = it->second.to.x.relative * width() + it->second.to.x.absolute;
        int y2a = it->second.to.y.relative * height() + it->second.to.y.absolute;
        
        if (x >= x1a && x <= x2a && y >= y1a && y <= y2a)
        {
            if (wX != nullptr)
                *wX = x1a;
            if (wY != nullptr)
                *wY = y1a;
            
            return it->first;
        }
    }
    
    return nullptr;
}

