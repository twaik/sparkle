#ifndef PLATFORM_X11_H
#define PLATFORM_X11_H

#include "platform/platform.h"
#include "were/were_event_loop.h"

Platform *platform_x11_create(WereEventLoop *loop);

#endif //PLATFORM_X11_H

