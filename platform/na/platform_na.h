#ifndef PLATFORM_NA_H
#define PLATFORM_NA_H

#include "platform/platform.h"
#include "were/were_event_loop.h"
#include <android_native_app_glue.h>

Platform *platform_na_create(WereEventLoop *loop, struct android_app *app);

#endif //PLATFORM_NA_H

