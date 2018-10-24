#include "were_timer.h"
#include <sys/timerfd.h>
#include <unistd.h>

/* ================================================================================================================== */

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
        throw WereException("[%p][%s] Failed to create timer fd.", this, __PRETTY_FUNCTION__);

    setBlocking(false);

    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

/* ================================================================================================================== */

void WereTimer::event(uint32_t events)
{
    if (events == EPOLLIN)
    {
        uint64_t expirations;

        if (read(_fd, &expirations, sizeof(uint64_t)) != sizeof(uint64_t))
            throw WereException("[%p][%s] Failed to read timer fd.", this, __PRETTY_FUNCTION__);

        timeout();
    }
    else
        throw WereException("[%p][%s] Unknown event type.", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */

void WereTimer::start(int interval, bool singleShot)
{
    if (interval <= 0)
        throw WereException("[%p][%s] Illegal interval.", this, __PRETTY_FUNCTION__);

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
        throw WereException("[%p][%s] Failed to start timer.", this, __PRETTY_FUNCTION__);
}

void WereTimer::stop()
{
    struct itimerspec new_value;

    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(_fd, 0, &new_value, NULL) == -1)
        throw WereException("[%p][%s] Failed to stop timer.", this, __PRETTY_FUNCTION__);
}

/* ================================================================================================================== */
