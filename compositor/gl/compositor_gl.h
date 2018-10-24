#ifndef COMPOSITOR_GL_H
#define COMPOSITOR_GL_H

#include "compositor/compositor.h"
#include "were/were_event_loop.h"
#include "platform/platform.h"

Compositor *compositor_gl_create(WereEventLoop *loop, Platform *platform,
    const std::string &file);

#endif //COMPOSITOR_GL_H
