#include "widget_button.h"
#include "painter.h"

WidgetButton::WidgetButton()
{
    _pressed = false;
    _label = 0;
}

void WidgetButton::draw(uint32_t *buffer, int stride, int width, int height)
{
    if (_pressed)
    {
        drawRectangle2(buffer, stride, 0, 0, width - 1, height - 1, 0xFFAAFFAA);
    }
    else
    {
        drawRectangle2(buffer, stride, 0, 0, width - 1, height - 1, 0xFFAAAAAA);
    }
    
    if (_label != 0)
    {
        drawString(buffer, stride, 2, 2, _label);
    }
}

void WidgetButton::pointerDown(int slot, int x, int y)
{
    pressed(this);
    _pressed = true;
    
    damage(this);
}

void WidgetButton::pointerUp(int slot, int x, int y)
{
    if (_pressed)
    {
        released(this);
    }
    _pressed = false;
    
    damage(this);
}

void WidgetButton::setLabel(const char *label)
{
    _label = label;
}
