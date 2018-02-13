#ifndef PLATFORM_H
#define PLATFORM_H

#include "were/were_function.h"
#include "sparkle.h"
#include <EGL/egl.h>

class Platform
{
public:
    virtual ~Platform() {}
    virtual int start() = 0;
    virtual int stop() = 0;

    WereFunction<int ()> getVID;
    WereSignal<void (NativeDisplayType)> initializeForNativeDisplay;
    WereSignal<void ()> finishForNativeDisplay;
    WereSignal<void (NativeWindowType)> initializeForNativeWindow;
    WereSignal<void ()> finishForNativeWindow;

    WereSignal<void ()> draw;

    WereSignal<void (int, int, int)> pointerDown;
    WereSignal<void (int, int, int)> pointerUp;
    WereSignal<void (int, int, int)> pointerMotion;
    WereSignal<void (int)> keyDown;
    WereSignal<void (int)> keyUp;
};

#endif //PLATFORM_H

