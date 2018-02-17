#include "were_signal_handler.h"
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

//==================================================================================================

WereSignalHandler::~WereSignalHandler()
{
    _loop->unregisterEventSource(this);
    
    close(_fd);
}

WereSignalHandler::WereSignalHandler(WereEventLoop *loop) :
    WereEventSource(loop)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
        
    _fd = signalfd(-1, &mask, 0);
    if (_fd == -1)
        throw WereException("[%p][%s] Failed to create signal fd.", this, __PRETTY_FUNCTION__);
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereSignalHandler::event(uint32_t events)
{
    if (events == EPOLLIN)
    {        
        struct signalfd_siginfo si;
        
        if (read(_fd, &si, sizeof(si)) != sizeof(si))
            throw WereException("[%p][%s] Failed to read signal fd.", this, __PRETTY_FUNCTION__);
        
        terminate();
    }
    else
        throw WereException("[%p][%s] Unknown event type.", this, __PRETTY_FUNCTION__);
}

//==================================================================================================

