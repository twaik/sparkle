#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "widget.h"

class WidgetButton : public Widget
{
public:
    WidgetButton();

    void draw(uint32_t *buffer, int stride, int width, int height);

    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    
    WFunctionL<void (WidgetButton *button)> pressed;
    WFunctionL<void (WidgetButton *button)> released;
    
    void setLabel(const char *label);
    
private:
    bool _pressed;
    const char *_label;
};

#endif //WIDGET_BUTTON_H

