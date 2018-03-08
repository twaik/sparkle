#include "were_button.h"
#include "were_painter.h"

WereButton::WereButton()
{
    _pressed = false;
    _label = 0;
}

void WereButton::draw()
{
    if (surface() == 0)
        return;
    
    WerePainter painter(this);
    
    painter.setColor(0xA0000000);
    painter.fill(0, 0, width() - 1, height() - 1);
    
    if (_pressed)
    {
        painter.setColor(0xFFAAFFAA);
        painter.drawRectangle(0, 0, width() - 1, height() - 1);
    }
    else
    {
        painter.setColor(0xFFAAAAAA);
        painter.drawRectangle(0, 0, width() - 1, height() - 1);
    }
    
    if (_label != 0)
    {
        int stringWidth;
        int stringHeight;
        painter.stringSize(_label, &stringWidth, &stringHeight);
        
        if (stringWidth <= width() - 4 && stringHeight <= height() - 4)
        {
            painter.drawString(2, 2, _label);
        }
    }
}

void WereButton::pointerDown(int slot, int x, int y)
{
    pressed();
    _pressed = true;
    
    draw();
}

void WereButton::pointerUp(int slot, int x, int y)
{
    if (_pressed)
    {
        released();
    }
    _pressed = false;
    
    draw();
}

void WereButton::setLabel(const char *label)
{
    _label = label;
    
    draw();
}

