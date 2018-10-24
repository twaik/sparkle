#include "were_event_source.h"
#include <fcntl.h>

/* ================================================================================================================== */

WereEventSource::~WereEventSource()
{
}

WereEventSource::WereEventSource(WereEventLoop *loop)
{
    _loop = loop;
    _fd = -1;
}

/* ================================================================================================================== */

int WereEventSource::fd()
{
    if (_fd == -1)
        throw WereException("[%p][%s] Bad fd.", this, __PRETTY_FUNCTION__);
    else
        return _fd;
}

void WereEventSource::setBlocking(bool blocking)
{
    if (!blocking)
    {
        int flags = fcntl(_fd, F_GETFL, 0);
        if (flags == -1)
            throw WereException("[%s] Failed to get fd flags.", __PRETTY_FUNCTION__);
        flags |= O_NONBLOCK;
        if (fcntl(_fd, F_SETFL, flags) == -1)
            throw WereException("[%s] Failed to set fd flags.", __PRETTY_FUNCTION__);
    }
}

/* ================================================================================================================== */
