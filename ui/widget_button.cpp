#include "widget_button.h"
#include "ui/were_painter.h"

WidgetButton::WidgetButton()
{
    _pressed = false;
    _label = 0;
}

void WidgetButton::draw(unsigned char *buffer, int stride, int width, int height)
{
    WerePainter painter(buffer, stride);
    
    painter.setColor(0xA0000000);
    painter.fill(0, 0, width - 1, height - 1);
    
    if (_pressed)
    {
        painter.setColor(0xFFAAFFAA);
        painter.drawRectangle2(0, 0, width - 1, height - 1);
    }
    else
    {
        painter.setColor(0xFFAAAAAA);
        painter.drawRectangle2(0, 0, width - 1, height - 1);
    }
    
    if (_label != 0)
    {
        int stringWidth;
        int stringHeight;
        painter.stringSize(_label, &stringWidth, &stringHeight);
        
        if (stringWidth <= width - 4 && stringHeight <= height - 4)
        {
            painter.drawString(2, 2, _label);
        }
    }
}

void WidgetButton::pointerDown(int slot, int x, int y)
{
    pressed();
    _pressed = true;
    
    damage();
}

void WidgetButton::pointerUp(int slot, int x, int y)
{
    if (_pressed)
    {
        released();
    }
    _pressed = false;
    
    damage();
}

void WidgetButton::setLabel(const char *label)
{
    _label = label;
}
