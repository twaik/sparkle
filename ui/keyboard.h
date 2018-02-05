#ifndef WINDOW_KEYBOARD_H
#define WINDOW_KEYBOARD_H

#include "window/host/window_host.h"
#include "widget/widget_button.h"

class KeyboardButton : public WidgetButton
{
public:
    void setCode(int code);
    int code();
    
private:
    int _code;
};

class WindowKeyboard : public WindowHost
{
public:
    ~WindowKeyboard();
    WindowKeyboard();
    
    WFunctionL<void (int code)> keyDown;
    WFunctionL<void (int code)> keyUp;
    
private:
    KeyboardButton *_buttons;
    
    void buttonPressed(WidgetButton *button);
    void buttonReleased(WidgetButton *button);
};

#endif //WINDOW_KEYBOARD_H

