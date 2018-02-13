#include "were_event_source.h"

//==================================================================================================

WereEventSource::~WereEventSource()
{
}

WereEventSource::WereEventSource(WereEventLoop *loop)
{
    _loop = loop;
    _fd = -1;
}

//==================================================================================================

int WereEventSource::fd()
{
    if (_fd == -1)
        throw std::runtime_error("[WereEventSource::fd] Bad fd.");
    else
        return _fd;
}

//==================================================================================================

