#ifndef WERE_EVENT_LOOP_H
#define WERE_EVENT_LOOP_H

/* ================================================================================================================== */

#include "were.h"
#include <sys/epoll.h>
#include <functional>
#include <vector>
#include <thread>

class WereEventSource;
class WereCallQueue;

class WereEventLoop
{
public:
    ~WereEventLoop();
    WereEventLoop();

    int fd();

    void registerEventSource(WereEventSource *source, uint32_t events);
    void unregisterEventSource(WereEventSource *source);

    void run();
    void runThread();
    void exit();

    void processEvents();

    void queue(const std::function<void ()> &f);

private:

private:
    int _epoll;
    bool _exit;

    WereCallQueue *_queue;

    std::thread _thread;
};

/* ================================================================================================================== */

#endif /* WERE_EVENT_LOOP_H */
