#include "were_button.h"
#include "were_painter.h"

WereButton::WereButton()
{
    _pressed = -1;
    _label = 0;
}

void WereButton::draw()
{
    if (surface() == 0)
        return;
    
    WerePainter painter(this);
    
    if (_pressed == -1)
        painter.setColor(0x00000000);
    else
        painter.setColor(0xFFAAFFAA);
    
    painter.fill(0, 0, width() - 1, height() - 1);
    
    if (_pressed == -1)
        painter.setColor(0xFFAAFFAA);
    else
        painter.setColor(0x00000000);
    
    painter.drawRectangle(1, 1, width() - 2, height() - 2);
    
    if (_label != 0)
    {
        int stringWidth;
        int stringHeight;
        painter.stringSize(_label, &stringWidth, &stringHeight);
        
        if (stringWidth <= width() - 6 && stringHeight <= height() - 6)
            painter.drawString(3, 3, _label);
    }
}

void WereButton::pointerDown(int slot, int x, int y)
{
    if (_pressed == -1)
    {
        _pressed = slot;
        pressed();
        draw();
    }
}

void WereButton::pointerUp(int slot, int x, int y)
{
    if (_pressed == slot)
    {
        _pressed = -1;
        released();
        draw();
    }
}

void WereButton::pointerMotion(int slot, int x, int y)
{
}

void WereButton::pointerEnter(int slot)
{
}

void WereButton::pointerLeave(int slot)
{
    if (_pressed == slot)
    {
        _pressed = -1;
        released();
        draw();
    }
}

void WereButton::setLabel(const char *label)
{
    _label = label;
    
    draw();
}

