#include "were_event_loop.h"
#include "were_event_source.h"
#include "were_call_queue.h"
#include <unistd.h>
#include <syscall.h>

/* ================================================================================================================== */

const int MAX_EVENTS = 16;

/* ================================================================================================================== */

WereEventLoop::~WereEventLoop()
{
    delete _queue;

    close(_epoll);
}

WereEventLoop::WereEventLoop()
{
    _epoll = epoll_create(1);
    if (_epoll == -1)
        throw WereException("[%p][%s] Failed to create epoll device.", this, __PRETTY_FUNCTION__);

    _exit = false;

    _queue = new WereCallQueue(this);
}

/* ================================================================================================================== */

int WereEventLoop::fd()
{
    return _epoll;
}

/* ================================================================================================================== */

void WereEventLoop::registerEventSource(WereEventSource *source, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = source;

    if (epoll_ctl(_epoll, EPOLL_CTL_ADD, source->fd(), &ev) == -1)
        throw WereException("[%p][%s] Failed to register event source.", this, __PRETTY_FUNCTION__);
}

void WereEventLoop::unregisterEventSource(WereEventSource *source)
{
    if (epoll_ctl(_epoll, EPOLL_CTL_DEL, source->fd(), NULL) == -1)
        throw WereException("[%p][%s] Failed to unregister event source.", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */

void WereEventLoop::run()
{
    were_debug("[%p][%s] Started (thread %ld).\n", this, __PRETTY_FUNCTION__, syscall(SYS_gettid));

    struct epoll_event events[MAX_EVENTS];

    while (!_exit)
    {
        int n = epoll_wait(_epoll, events, MAX_EVENTS, -1);
        if (n == -1)
            throw WereException("[%p][%s] epoll_wait returned -1.", this, __PRETTY_FUNCTION__);

        for (int i = 0; i < n; ++i)
        {
            WereEventSource *source = static_cast<WereEventSource *>(events[i].data.ptr);
            source->event(events[i].events);
        }
    }

    were_debug("[%p][%s] Finished (thread %ld).\n", this, __PRETTY_FUNCTION__, syscall(SYS_gettid));
}

void WereEventLoop::runThread()
{
    _thread = std::thread(&WereEventLoop::run, this);
}

void WereEventLoop::exit()
{
    /* FIXME Break epoll_wait */
    _exit = true;
}

void WereEventLoop::processEvents()
{
    struct epoll_event events[MAX_EVENTS];

    int n = epoll_wait(_epoll, events, MAX_EVENTS, 0);
    if (n == -1)
        throw WereException("[%p][%s] epoll_wait returned -1.", this, __PRETTY_FUNCTION__);

    for (int i = 0; i < n; ++i)
    {
        WereEventSource *source = static_cast<WereEventSource *>(events[i].data.ptr);
        source->event(events[i].events);
    }
}

void WereEventLoop::queue(const std::function<void ()> &f)
{
    _queue->queue(f);
}

/* ================================================================================================================== */
