#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "widget.h"

class WidgetButton : public Widget
{
public:
    WidgetButton();

    void draw(unsigned char *buffer, int stride, int width, int height);

    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    
    WereSignal<void ()> pressed;
    WereSignal<void ()> released;
    
    void setLabel(const char *label);
    
private:
    bool _pressed;
    const char *_label;
};

#endif //WIDGET_BUTTON_H

