#ifndef WERE_CALL_QUEUE_H
#define WERE_CALL_QUEUE_H

#include "were.h"
#include "were_event_source.h"
#include <functional>
#include <vector>
#include <queue>

/* ================================================================================================================== */

class WereCallQueue : public WereEventSource
{
public:
    ~WereCallQueue();
    WereCallQueue(WereEventLoop *loop);

    void queue(const std::function<void ()> &f);

private:
    void event(uint32_t events);

private:
    std::queue< std::function<void ()> > _functions;
};

/* ================================================================================================================== */

#endif /* WERE_CALL_QUEUE_H */
