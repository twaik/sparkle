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
        throw WereException("[%p][%s] Bad fd.", this, __PRETTY_FUNCTION__);
    else
        return _fd;
}

//==================================================================================================

