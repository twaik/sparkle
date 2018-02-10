#include "were_signal_handler.h"
#include <stdexcept>
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
        throw std::runtime_error("[WereSignalHandler::WereSignalHandler] Failed to create signal fd.");
    
    _loop->registerEventSource(this, EPOLLIN | EPOLLET);
}

//==================================================================================================

void WereSignalHandler::event(uint32_t events)
{
    if (events == EPOLLIN)
    {        
        struct signalfd_siginfo si;
        read(_fd, &si, sizeof(si));
        
        terminate();
    }
    else
    {
        throw std::runtime_error("[WereSignalHandler::event] Unknown event type.");
    }
}

//==================================================================================================

