#ifndef WERE_BUTTON_H
#define WERE_BUTTON_H

#include "were/were_signal.h"
#include "were_widget.h"

class WereButton : public WereWidget
{
public:
    WereButton();

    void draw();

    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    
    WereSignal<void ()> pressed;
    WereSignal<void ()> released;
    
    void setLabel(const char *label);
    
private:
    bool _pressed;
    const char *_label;
};

#endif //WERE_BUTTON_H

