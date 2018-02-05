#include "were_call_queue.h"
#include <stdexcept>
#include <sys/eventfd.h>
#include <unistd.h>

//==================================================================================================

WereCallQueue::~WereCallQueue()
{
    _loop->unregisterEventSource(this);
    
    close(_fd);
}

WereCallQueue::WereCallQueue(WereEventLoop *loop) :
    WereEventSource(loop)
{
    _fd = eventfd(0, 0); //EFD_SEMAPHORE
    if (_fd == -1)
        throw std::runtime_error("[WereCallQueue::WereCallQueue] Failed to create event fd.");
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereCallQueue::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t counter = 0;
        read(_fd, &counter, sizeof(uint64_t));
        
        //fprintf(stdout, "calling functions %lu/%lu\n", counter, _functions.size());
        
        //FIXME lock
        for (unsigned int i = 0; i < counter; ++i)
        {
            _functions.front()();
            _functions.pop();
        }
        //FIXME unlock
    }
    else
    {
        throw std::runtime_error("[WereCallQueue::event] Unknown event type.");
    }
}

//==================================================================================================

void WereCallQueue::queue(const std::function<void ()> &f)
{
    //FIXME lock
    _functions.push(f);
    //FIXME unlock
    uint64_t add = 1;
    write(_fd, &add, sizeof(uint64_t));
}

//==================================================================================================

