#ifndef WERE_TIMER_H
#define WERE_TIMER_H

#include "were.h"
#include "were_event_source.h"
#include "were_signal.h"


class WereTimer : public WereEventSource
{
public:
    ~WereTimer();
    WereTimer(WereEventLoop *loop);
    
    void start(int interval, bool singleShot);
    void stop();
    
    WereSignal<void ()> timeout;
    
private:
    void event(uint32_t events);
};


#endif //WERE_TIMER_H

