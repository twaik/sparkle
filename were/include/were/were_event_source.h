#ifndef WERE_EVENT_SOURCE_H
#define WERE_EVENT_SOURCE_H

#include "were.h"
#include "were_event_loop.h"

/* ================================================================================================================== */

class WereEventSource
{
    friend class WereEventLoop;
public:
    virtual ~WereEventSource();
    WereEventSource(WereEventLoop *loop);

    int fd();

    void setBlocking(bool blocking);

private:
    virtual void event(uint32_t events) = 0;

protected:
    WereEventLoop *_loop;
    int _fd;
};

/* ================================================================================================================== */

#endif /* WERE_EVENT_SOURCE_H */
