#include "were_timer.h"
#include <stdexcept>
#include <sys/timerfd.h>
#include <unistd.h>

//==================================================================================================

WereTimer::~WereTimer()
{
    _loop->unregisterEventSource(this);
    
    close(_fd);
}

WereTimer::WereTimer(WereEventLoop *loop) :
    WereEventSource(loop)
{
    _fd = timerfd_create(CLOCK_REALTIME, 0);
    if (_fd == -1)
        throw std::runtime_error("[WereTimer::WereTimer] Failed to create timer.");
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereTimer::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t expirations;
    
        if (read(_fd, &expirations, sizeof(uint64_t)) != sizeof(uint64_t))
            throw std::runtime_error("[WereTimer::handleData] Failed to read timer.");
        
        timeout();
    }
}
    
//==================================================================================================    

void WereTimer::start(int interval, bool singleShot)
{
    if (interval <= 0)
        throw std::runtime_error("[WereTimer::start] Illegal interval.");

    struct itimerspec new_value;
    
    new_value.it_value.tv_sec = interval / 1000;
    new_value.it_value.tv_nsec = (interval % 1000) * 1000000;
    
    if (singleShot)
    {
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = 0;
    }
    else
    {
        new_value.it_interval.tv_sec = interval / 1000;
        new_value.it_interval.tv_nsec = (interval % 1000) * 1000000;
    }
    
    if (timerfd_settime(_fd, 0, &new_value, NULL) == -1)
        throw std::runtime_error("[WereTimer::start] Failed to start timer.");
}

void WereTimer::stop()
{
    struct itimerspec new_value;
    
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(_fd, 0, &new_value, NULL) == -1)
        throw std::runtime_error("[WereTimer::stop] Failed to stop timer.");
}

//==================================================================================================

