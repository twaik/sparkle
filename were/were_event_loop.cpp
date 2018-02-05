#include "were_event_loop.h"
#include "were_event_source.h"
#include "were_call_queue.h"
#include <stdexcept>
#include <unistd.h>

//==================================================================================================

const int MAX_EVENTS = 16;

//==================================================================================================

WereEventLoop::~WereEventLoop()
{
    delete _queue;
    
    close(_epoll);
}

WereEventLoop::WereEventLoop()
{
    _epoll = epoll_create(1);
    if (_epoll == -1)
        throw std::runtime_error("[WereEventLoop::WereEventLoop] Failed to create epoll device.");
    
    _exit = false;
    
    _queue = new WereCallQueue(this);
}

//==================================================================================================

int WereEventLoop::fd()
{
    return _epoll;
}

//==================================================================================================

void WereEventLoop::registerEventSource(WereEventSource *source, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = source;

    if (epoll_ctl(_epoll, EPOLL_CTL_ADD, source->fd(), &ev) == -1)
        throw std::runtime_error("[WereEventLoop::registerEventSource] Failed to register event source.");
}

void WereEventLoop::unregisterEventSource(WereEventSource *source)
{
    if (epoll_ctl(_epoll, EPOLL_CTL_DEL, source->fd(), NULL) == -1)
        throw std::runtime_error("[WereEventLoop::unregisterEventSource] Failed to unregister event source.");
}
    
//==================================================================================================

void WereEventLoop::run()
{
    struct epoll_event events[MAX_EVENTS];

    while (!_exit)
    {
        int n = epoll_wait(_epoll, events, MAX_EVENTS, -1);
        if (n == -1)
            throw std::runtime_error("[WereEventLoop::run] epoll_wait returned -1.");

        for (int i = 0; i < n; ++i)
        {
            WereEventSource *source = static_cast<WereEventSource *>(events[i].data.ptr);
            
            source->event(events[i].events);
        }
    }
}

void WereEventLoop::exit()
{
    _exit = true;
}

void WereEventLoop::processEvents()
{
    struct epoll_event events[MAX_EVENTS];
    
    int n = epoll_wait(_epoll, events, MAX_EVENTS, 0);
    if (n == -1)
        throw std::runtime_error("[WereEventLoop::processEvents] epoll_wait returned -1.");

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

//==================================================================================================

were_event_loop_t *were_event_loop_create()
{
    WereEventLoop *_loop = new WereEventLoop();
    
    return static_cast<were_event_loop_t *>(_loop);
}

void were_event_loop_destroy(were_event_loop_t *loop)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    
    delete _loop;
}

void were_event_loop_process_events(were_event_loop_t *loop)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    
    _loop->processEvents();
}

int were_event_loop_fd(were_event_loop_t *loop)
{
    WereEventLoop *_loop = static_cast<WereEventLoop *>(loop);
    
    return _loop->fd();
}

//==================================================================================================

